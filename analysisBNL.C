
void analysisBNL(TString volt = "200", bool local = false){
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
    TFile *output = TFile::Open("BNL2020_"+volt+"_A.root","RECREATE");

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

    // Definition of branches and leaves
    chain->SetBranchStatus("*",0);
    chain->SetBranchStatus("amp",1);
    chain->SetBranchStatus("LP2_20",1);
    chain->SetBranchStatus("x_dut",1);
    chain->SetBranchStatus("y_dut",1);

    float amp[8], LP2_20[8], x_dut[4], y_dut[4];

    chain->SetBranchAddress("amp",amp);
    chain->SetBranchAddress("LP2_20",LP2_20);
    chain->SetBranchAddress("x_dut",x_dut);
    chain->SetBranchAddress("y_dut",y_dut);

    // Amp threshold value and position of wires
    int amp_Low_Cut[8] = {60, 60, 60, 70, 70, 70, 70, 70};
    int amp_High_Cut[8] = {220, 200, 200, 200, 180, 180, 180, 350};

    float xy_Coord[4] = {-1.1, 1.5, 9.5, 12.2}; // {x_min, x_max, y_min, y_max}
    float ch_Limits[6][4] = {{0.29, 0.41, 10.0, 11.6}, {0.20, 0.30, 10, 11.6}, {0.10, 0.20, 10, 11.6},
                            {0.00, 0.10, 10.0, 11.6}, {-0.10, -0.01, 10, 11.6}, {-0.21, -0.11, 10, 11.6}};

    // Create histograms
    std::vector<TH1F*> hamp_Vec;
    std::vector<TH2F*> hxy_Vec;
    std::vector<TH1F*> hamp_ChCut_Vec;
    std::vector<TH1F*> ht_Res0_Vec; // Resolution without zoom
    std::vector<TH1F*> ht_Res1_Vec; // Resolution with zoom, but no cuts
    std::vector<TH1F*> ht_Res2_Vec; // Resolution with zoom and all cuts
    std::vector<TH2F*> ht_Mean_Vec;
    std::vector<TH2F*> hamp_Frac_Vec;

    for (int ch=0; ch<8; ch++){
        auto htitle_amp = Form("amp[%i];amp[%i] [mV];Counts",ch,ch);
        TH1F *hamp = new TH1F(Form("hamp%i",ch),htitle_amp,400,0,400);
        hamp_Vec.push_back(hamp);

        auto htitle_xy = Form("Hits in channel %i with %i<amp[%i]<%i;x_dut [mm];y_dut [mm]",ch,amp_Low_Cut[ch],ch,amp_High_Cut[ch]);
        TH2F *hxy = new TH2F(Form("hxy%i",ch),htitle_xy,100,xy_Coord[0],xy_Coord[1],10,xy_Coord[2],xy_Coord[3]);
        hxy_Vec.push_back(hxy);

        if (ch==7 || ch==0) continue;
        auto htitle_ampChCut = Form("amp[%i] only channel;amp[%i] [mV];Counts",ch,ch);
        TH1F *hamp_ChCut = new TH1F(Form("hamp%i_chcut",ch),htitle_ampChCut,400,0,400);
        hamp_ChCut->SetLineColor(kRed);
        hamp_ChCut_Vec.push_back(hamp_ChCut);

        // Time resolution
        auto htitle_tRes = Form("#Delta t = time[7] - time[%i];#Delta t [ns];Counts",ch);
        TH1F *ht_Res0 = new TH1F(Form("ht_Res%i0",ch),htitle_tRes,100,0,0);
        ht_Res0_Vec.push_back(ht_Res0); // Draw("LP2_20[7]*1e9-LP2_20[ch]*1e9>>tRes%i0","LP2_20[ch]!=0 && LP2_20[7]!=0");
        TH1F *ht_Res1 = new TH1F(Form("ht_Res%i1",ch),htitle_tRes,100,9.5,11.5);
        ht_Res1_Vec.push_back(ht_Res1); // Draw("LP2_20[7]*1e9-LP2_20[ch]*1e9>>tRes%i1","LP2_20[ch]!=0 && LP2_20[7]!=0");
        TH1F *ht_Res2 = new TH1F(Form("ht_Res%i2",ch),htitle_tRes,100,9.5,11.5);
        ht_Res2_Vec.push_back(ht_Res2);
        // Draw("LP2_20[7]*1e9-LP2_20[ch]*1e9>>tRes%i1","LP2_20[ch]!=0 && LP2_20[7]!=0"+xch_cut+ych_cut+ampCut_Low+ampCut_High);
        
        auto htitle_tMean = Form("#Delta t = time[7] - time[%i] vs x_dut;x_dut [mm];#Delta t [ns]",ch);
        TH2F *ht_Mean = new TH2F(Form("ht_Mean%i",ch),htitle_tMean,100,xy_Coord[0],xy_Coord[1],100,9.5,11.5);
        // Draw("LP2_20[7]*1e9-LP2_20[%i]*1e9:x_dut[0]>>tMean%i2","LP2_20[%i]!=0 && LP2_20[7]!=0"+xCut+ych_cut+ampCut_Low+ampCut_High);
        ht_Mean_Vec.push_back(ht_Mean);

        auto htitle_ampFrac = Form("amp[%i]/Sum over channels 1-6 vs x_dut;x_dut [mm];amp %%",ch);
        TH2F *hamp_Frac = new TH2F(Form("hamp_Frac%i",ch),htitle_ampFrac,100,xy_Coord[0],xy_Coord[1],100,0.,1.);
        hamp_Frac_Vec.push_back(hamp_Frac);
    }

    TH1F *hamp_Tot_0 = new TH1F("hamp_Tot_0","amp for channels 0-6;amp [mV];Counts",400,0,400);
    TH1F *hamp_Tot_1 = new TH1F("hamp_Tot_1","amp for channels 1-6;amp [mV];Counts",400,0,400);
    TH1F *hamp_Tot_A = new TH1F("hamp_Tot_A","amp for channels 0-7;amp [mV];Counts",400,0,400);
    TH1F *hamp_Sum_0 = new TH1F("hamp_Sum_0","Sum of amp for channels 0-6;amp [mV];Counts",400,0,400);
    TH1F *hamp_Sum_1 = new TH1F("hamp_Sum_1","Sum of amp for channels 1-6;amp [mV];Counts",400,0,400);
    TH1F *hamp_Sum_LP = new TH1F("hamp_Sum_LP","Sum of amp for channels 1-6 only when LP!=0;amp [mV];Counts",400,0,400);
    TH1F *hamp_Tot_Cut = new TH1F("hamp_Tot_Cut","amp for channels 1-6, with spatial cuts;amp [mV];Counts",400,0,400);
    hamp_Tot_Cut->SetLineColor(kRed);
    TH2F *hxy_Sum1 = new TH2F("hxy_Sum1","Sum of channels 1 to 6;x_dut [mm];y_dut [mm]",100,xy_Coord[0],xy_Coord[1],
                                10,xy_Coord[2],xy_Coord[3]);
    TH2F *ht_MeanTot = new TH2F("ht_MeanTot","#Delta t vs x_dut;x_dut [mm];#Delta t [ns]",
                                   100,xy_Coord[0],xy_Coord[1],100,9.5,11.5);
    auto htitle_tweight_LP = "Weighted time with LP!=0 vs x_dut;x_dut [mm]; time[7] - weighted_time [ns]";
    TH2F *ht_weight_LP = new TH2F("ht_weight_LP",htitle_tweight_LP,100,xy_Coord[0],xy_Coord[1],100,9.5,11.5);

    //// std::vector<TH1F*> hamp_Vec; // without cuts
    //// std::vector<TH2F*> hxy_Vec;
    //// std::vector<TH1F*> hamp_ChCut_Vec; // channel cuts
    //// std::vector<TH1F*> ht_Res0_Vec; // Resolution without zoom
    //// std::vector<TH1F*> ht_Res1_Vec; // Resolution with zoom, but no cuts
    //// std::vector<TH1F*> ht_Res2_Vec; // Resolution with zoom and all cuts (per channel)
    //// std::vector<TH2F*> ht_Mean_Vec;
    //// std::vector<TH2F*> hamp_Frac_Vec;
    //// TH1F *hamp_Tot_0 // channels 0-6
    //// TH1F *hamp_Tot_1 // channels 1-6
    //// TH1F *hamp_Tot_A // channels 0-7 (All)
    //// TH1F *hamp_Sum_0 // Sum of channels 0-6
    //// TH1F *hamp_Sum_1 // Sum of channels 1-6
    //// TH1F *hamp_Sum_LP // Sum of channels 1-6 with LP!=0
    //// TH1F *hamp_Tot_Cut // Spatial and amp cuts
    //// TH2F *hxy_Sum1
    //// TH2F *ht_MeanTot
    //// TH2F *ht_weight_LP

    // Loop over all entries
    for (int i=0; i<Nentries; i++){
        chain->GetEntry(i);

        bool cut_Coord = false;
        if (x_dut[0]>xy_Coord[0] && x_dut[0]<xy_Coord[1] && y_dut[0]>xy_Coord[2] && y_dut[0]<xy_Coord[3]) cut_Coord = true;
        if (!cut_Coord) continue;

        float sum_amp_0 = 0.0;
        float sum_amp_1 = 0.0;
        float sum_amp_time_1 = 0.0;
        float weight_numerator = 0.0;

        for (int ch=0; ch<8; ch++){
            if (amp[ch]<0) continue;

            bool cut_amp = false;
            if (amp[ch]>amp_Low_Cut[ch] && amp[ch]<amp_High_Cut[ch]) cut_amp = true;

            // Fill amp for all channels
            hamp_Vec[ch]->Fill(amp[ch]);
            hamp_Tot_A->Fill(amp[ch]);

            // Fill spatial hists for all channels
            if (cut_amp) hxy_Vec[ch]->Fill(x_dut[0],y_dut[0]);

            // Fill considering different channels
            if (ch==0){
                // Amp plots
                hamp_Tot_0->Fill(amp[ch]);
                sum_amp_0+=amp[ch];

                // Spatial plots
            }
            else if (ch==7){
                // Amp plots

                // Spatial plots
            }
            else{
                bool cut_Channel = false;
                if (x_dut[0]>ch_Limits[ch-1][0] && x_dut[0]<ch_Limits[ch-1][1] &&
                    y_dut[0]>ch_Limits[ch-1][2] && y_dut[0]<ch_Limits[ch-1][3]) cut_Channel = true;

                // Amp plots
                hamp_Tot_0->Fill(amp[ch]);
                hamp_Tot_1->Fill(amp[ch]);
                if (cut_Channel){
                    hamp_ChCut_Vec[ch-1]->Fill(amp[ch]);
                    hamp_Tot_Cut->Fill(amp[ch]);
                }
                sum_amp_0+=amp[ch];
                sum_amp_1+=amp[ch];

                // Spatial plots
                if (cut_amp){
                    hxy_Sum1->Fill(x_dut[0],y_dut[0]);
                }

                // Time plots
                if (LP2_20[ch]!=0){
                    sum_amp_time_1+=amp[ch];
                    weight_numerator+=amp[ch]*LP2_20[ch];
                }
                if (LP2_20[ch]!=0 && LP2_20[7]!=0){
                    ht_Res0_Vec[ch-1]->Fill((LP2_20[7]-LP2_20[ch])*1e9);
                    ht_Res1_Vec[ch-1]->Fill((LP2_20[7]-LP2_20[ch])*1e9);
                    if (cut_amp){
                        if (cut_Channel) ht_Res2_Vec[ch-1]->Fill((LP2_20[7]-LP2_20[ch])*1e9);
                        ht_Mean_Vec[ch-1]->Fill(x_dut[0],(LP2_20[7]-LP2_20[ch])*1e9);
                        ht_MeanTot->Fill(x_dut[0],(LP2_20[7]-LP2_20[ch])*1e9);
                    }
                }
            }
        }

        hamp_Sum_0->Fill(sum_amp_0);
        hamp_Sum_1->Fill(sum_amp_1);
        hamp_Sum_LP->Fill(sum_amp_time_1);

        if (sum_amp_1 > 110){
            for (int ch=1; ch<=6; ch++){
                if (amp[ch]<0) continue;

                hamp_Frac_Vec[ch-1]->Fill(x_dut[0],amp[ch]/sum_amp_1);
            }
        }
        if (sum_amp_time_1 > 110 && LP2_20[7]!=0){
            float weighted_time = weight_numerator/sum_amp_time_1;
            ht_weight_LP->Fill(x_dut[0],(LP2_20[7]-weighted_time)*1e9);
        }
    }

    // Save and close
    output->Write();
    output->Close();
}
