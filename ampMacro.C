
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
    int amp_low_cut[7] = {60, 60, 60, 70, 70, 70, 70};
    float xy_position[4] = {-1.1, 1.5, 9.5, 12.2}; // {x_min, x_max, y_min, y_max}

    TH2F *histxy_st0 = new TH2F("histxy_st0","Stack of channels 0 to 6",100,xy_position[0],xy_position[1],
                                10,xy_position[2],xy_position[3]);
    TH2F *histxy_st1 = new TH2F("histxy_st1","Stack of channels 1 to 6",100,xy_position[0],xy_position[1],
                                10,xy_position[2],xy_position[3]);

    for (int channel=0; channel<7; channel++){
        // Simple amp plot
        auto htitleamp = Form("amp[%i];amp[%i] [mV];Counts",channel,channel);
        TH1F *histamp = new TH1F(Form("amp%i",channel),htitleamp,100,0,0);
        chain->Draw(Form("amp[%i]>>amp%i",channel,channel),Form("amp[%i]>0",channel)); // && amp[%i]<100

        // x_dut vs y_dut
        TCut xCut = Form("x_dut[0]>%.1f && x_dut[0]<%.1f",xy_position[0],xy_position[1]);
        TCut yCut = Form("y_dut[0]>%.1f && y_dut[0]<%.1f",xy_position[2],xy_position[3]);
        TCut ampCut = Form("amp[%i]>%i",channel,amp_low_cut[channel]);

        auto htitlexy = Form("Hits in channel %i with amp[%i]>%i;x_dut [mm];y_dut [mm]",channel,channel,amp_low_cut[channel]);
        TH2F *histxy = new TH2F(Form("ampxy%i",channel),htitlexy,100,xy_position[0],xy_position[1],
                                10,xy_position[2],xy_position[3]);
        chain->Draw(Form("y_dut[0]:x_dut[0]>>ampxy%i",channel),xCut+yCut+ampCut,"COLZ");
        histxy_st0->Add(histxy);
        if (channel!=0) histxy_st1->Add(histxy);

        // Amp plot regarding channel only
        if (channel==0) continue;
        float x_mean = histxy->GetMean(1);
        float x_std = histxy->GetStdDev(1);
        float y_mean = histxy->GetMean(2);
        float y_std = histxy->GetStdDev(2);

        float ch_limits[4] = {x_mean-x_std, x_mean+x_std, y_mean-y_std, y_mean+y_std};

        auto htitleamp_chcut = Form("amp[%i] only channel;amp[%i] [mV];Counts",channel,channel);
        TH1F *histamp_chcut = new TH1F(Form("amp%i_chcut",channel),htitleamp_chcut,100,0,0);
        histamp_chcut->SetLineColor(kRed);
        TCut ch_cut = Form("x_dut[0]>%f && x_dut[0]<%f && y_dut[0]>%f && y_dut[0]<%f",ch_limits[0],
                           ch_limits[1],ch_limits[2],ch_limits[3]);
        chain->Draw(Form("amp[%i]>>amp%i_chcut",channel,channel),ampCut+ch_cut); // && amp[%i]<100
    }

    // Close and save!
    output->Write();
    output->Close();
}
