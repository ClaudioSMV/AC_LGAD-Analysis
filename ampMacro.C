
void ampMacro(TString volt = "200", bool local = false){
    TString num;
    if (volt == "200") num = "274";
    else if (volt == "210") num = "271";
    else if (volt == "220") num = "272";
    else if (volt == "225") num = "273";
    else {
        std::cerr << "[ERROR] wrong input value!" << std::endl;
        return -1;
    }

    // Input/Output
    TString txt = "../run-files/BNL2020_"+volt+"V_"+num+"_rootFileList.txt";
    std::ifstream ifile(txt);

    TChain *chain = new TChain("pulse");

    TFile *output = TFile::Open("BNL2020_"+volt+".root","RECREATE");

    // Reading files
    int counter = 0;
    if (ifile.is_open()){
        while (ifile.good()){
            TString line;
            ifile >> line;

            if (ifile.eof()) break;

            if (local) line = "../AC_LGAD-data/"+line;
            chain->Add(line);
            counter++;
        }
        ifile.close();
    }
    else {
        std::cerr << "[WARNING] input file NOT found!" << std::endl;
        return -1;
    }

    int Nentries = chain->GetEntries();
    std::cout << "Total number of files read: " << counter << std::endl;
    std::cout << "Total number of entries: " << Nentries << std::endl;

    // Amp threshold value and position of wires 
    int amp_low_cut[8] = {60, 60, 60, 70, 70, 70, 70, 70};
    int amp_high_cut[8] = {220, 200, 200, 200, 180, 180, 180, 350};
    float xy_position[4] = {-1.1, 1.5, 9.5, 12.2}; // {x_min, x_max, y_min, y_max}
    float ch_limits[6][4] = {{0.29, 0.41, 10.0, 11.6}, {0.20, 0.30, 10, 11.6}, {0.10, 0.20, 10, 11.6},
                            {0.00, 0.10, 10.0, 11.6}, {-0.10, -0.01, 10, 11.6}, {-0.21, -0.11, 10, 11.6}};

    TH1F *hamp_Tot = new TH1F("amp_Tot","amp for channels 1-6;amp [mV];Counts",400,0,400);
    TH1F *hamp_Tot_chcut = new TH1F("amp_Tot_chcut","amp for channels 1-6, with spatial cut;amp [mV];Counts",400,0,400);
    TH2F *histxy_st1 = new TH2F("histxy_st1","Stack of channels 1 to 6",100,xy_position[0],xy_position[1],
                                10,xy_position[2],xy_position[3]);
    TH2F *h_tMeanMovTot = new TH2F("tMean_Tot","#Delta t vs x_dut[0];x_dut[0] [mm];#Delta t [ns]",
                                   100,xy_position[0],xy_position[1],100,9.5,11.5);

    for (int channel=0; channel<8; channel++){
        // Simple amp plot
        auto htitleamp = Form("amp[%i];amp[%i] [mV];Counts",channel,channel);
        TH1F *histamp = new TH1F(Form("amp%i",channel),htitleamp,400,0,400);
        chain->Draw(Form("amp[%i]>>amp%i",channel,channel),Form("amp[%i]>0",channel)); // && amp[%i]<100
        if (channel!=0 && channel!=7) hamp_Tot->Add(histamp);

        // x_dut vs y_dut
        if (channel==7){
            xy_position[0] = -1.8;
            xy_position[1] = 1.8;
            xy_position[2] = 9.0;
            xy_position[3] = 13.0;
        }

        TCut xCut = Form("x_dut[0]>%.1f && x_dut[0]<%.1f",xy_position[0],xy_position[1]);
        TCut yCut = Form("y_dut[0]>%.1f && y_dut[0]<%.1f",xy_position[2],xy_position[3]);
        TCut ampCut_Low = Form("amp[%i]>%i",channel,amp_low_cut[channel]);
        TCut ampCut_High = Form("amp[%i]<%i",channel,amp_high_cut[channel]);

        auto htitlexy = Form("Hits in channel %i with amp[%i]>%i;x_dut [mm];y_dut [mm]",channel,channel,amp_low_cut[channel]);
        TH2F *histxy = new TH2F(Form("ampxy%i",channel),htitlexy,100,xy_position[0],xy_position[1],
                                10,xy_position[2],xy_position[3]);
        chain->Draw(Form("y_dut[0]:x_dut[0]>>ampxy%i",channel),xCut+yCut+ampCut_Low,"COLZ");
        if (channel!=0 && channel!=7) histxy_st1->Add(histxy);

        auto htitlexy_hi = Form("Hits in channel %i with %i<amp[%i]<%i;x_dut [mm];y_dut [mm]",channel,amp_low_cut[channel],channel,amp_high_cut[channel]);
        TH2F *histxy_hi = new TH2F(Form("ampxy%i_hi",channel),htitlexy_hi,100,xy_position[0],xy_position[1],
                                10,xy_position[2],xy_position[3]);
        chain->Draw(Form("y_dut[0]:x_dut[0]>>ampxy%i_hi",channel),xCut+yCut+ampCut_Low+ampCut_High,"COLZ");

        if (channel!=0 && channel!=7){
            // Amp plot regarding channel only
            TCut xch_cut = Form("x_dut[0]>%f && x_dut[0]<%f",ch_limits[channel-1][0],ch_limits[channel-1][1]);
            TCut ych_cut = Form("y_dut[0]>%f && y_dut[0]<%f",ch_limits[channel-1][2],ch_limits[channel-1][3]);
            auto htitleamp_chcut = Form("amp[%i] only channel;amp[%i] [mV];Counts",channel,channel);
            TH1F *histamp_chcut = new TH1F(Form("amp%i_chcut",channel),htitleamp_chcut,400,0,400);
            histamp_chcut->SetLineColor(kRed);
            chain->Draw(Form("amp[%i]>>amp%i_chcut",channel,channel),Form("amp[%i]>0",channel)+xch_cut+ych_cut);

            hamp_Tot_chcut->Add(histamp_chcut);

            // Time resolution
            auto htitle_tRes = Form("#Delta t = time[7] - time[%i];#Delta t [ns];Counts",channel);
            TH1F *hist_tRes1 = new TH1F(Form("tRes%i1",channel),htitle_tRes,100,0,0);
            chain->Draw(Form("LP2_20[7]*1e9-LP2_20[%i]*1e9>>tRes%i1",channel,channel),
                        Form("LP2_20[%i]!=0 && LP2_20[7]!=0",channel));
            TH1F *hist_tRes2 = new TH1F(Form("tRes%i2",channel),htitle_tRes,100,9.5,11.5);
            chain->Draw(Form("LP2_20[7]*1e9-LP2_20[%i]*1e9>>tRes%i2",channel,channel),
                        Form("LP2_20[%i]!=0 && LP2_20[7]!=0",channel));
            TH1F *hist_tRes3 = new TH1F(Form("tRes%i3",channel),htitle_tRes,100,9.5,11.5);
            chain->Draw(Form("LP2_20[7]*1e9-LP2_20[%i]*1e9>>tRes%i3",channel,channel),
                        Form("LP2_20[%i]!=0 && LP2_20[7]!=0"+xch_cut+ych_cut,channel));
            TH1F *hist_tRes4 = new TH1F(Form("tRes%i4",channel),htitle_tRes,100,9.5,11.5);
            chain->Draw(Form("LP2_20[7]*1e9-LP2_20[%i]*1e9>>tRes%i4",channel,channel),
                        Form("LP2_20[%i]!=0 && LP2_20[7]!=0"+xch_cut+ych_cut+ampCut_Low+ampCut_High,channel));
            
            auto htitle_tMean = Form("#Delta t = time[7] - time[%i] vs x_dut;x_dut [mm];#Delta t [ns]",channel);
            TH2F *h_tMeanMove1 = new TH2F(Form("tMean%i1",channel),htitle_tMean,100,xy_position[0],xy_position[1],100,9.5,11.5);
            chain->Draw(Form("LP2_20[7]*1e9-LP2_20[%i]*1e9:x_dut[0]>>tMean%i1",channel,channel),
                        Form("LP2_20[%i]!=0 && LP2_20[7]!=0"+xCut+ych_cut,channel));
            TH2F *h_tMeanMove2 = new TH2F(Form("tMean%i2",channel),htitle_tMean,100,xy_position[0],xy_position[1],100,9.5,11.5);
            chain->Draw(Form("LP2_20[7]*1e9-LP2_20[%i]*1e9:x_dut[0]>>tMean%i2",channel,channel),
                        Form("LP2_20[%i]!=0 && LP2_20[7]!=0"+xCut+ych_cut+ampCut_Low+ampCut_High,channel));
            
            h_tMeanMovTot->Add(h_tMeanMove2);
        }
    }

    hamp_Tot_chcut->SetLineColor(kRed);

    // Close and save!
    output->Write();
    output->Close();
}
