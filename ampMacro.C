
void ampMacro(TString volt = "200"){
    
    TString num;
    if (volt == "200") num = "274";
    else if (volt == "210") num = "271";
    else if (volt == "220") num = "272";
    else if (volt == "225") num = "273";
    else {
        std::cerr << "[ERROR] wrong input value!" << std::endl;
        return -1;
    }

    // TString txt = "/user/s/sanmartin/work/AC_LGAD/run-files/BNL2020_"+volt+"V_"+num+"_rootFileList.txt";
    TString txt = "/home/claudio/work/ac-lgad/AC_LGAD-data/list_runs.txt";
    std::ifstream ifile(txt);

    TChain *chain = new TChain("pulse");

    int counter = 0;
    if (ifile.is_open()){
        while (ifile.good()){
            TString line;
            ifile >> line;

            if (ifile.eof()) break;

            chain->Add("/home/claudio/work/ac-lgad/AC_LGAD-data/"+line);
            counter++;
        }
        ifile.close();
    }
    else {
        std::cerr << "[WARNING] input file NOT found!" << std::endl;
    }

    int Nentries = chain->GetEntries();
    std::cout << "Total number of files read: " << counter << std::endl;
    std::cout << "Total number of entries: " << Nentries << std::endl;

    // TString amp_low_cut[7] = {"15", "12", "11", "12", "12", "10", "11"};
    int amp_low_cut[7] = {15, 12, 11, 12, 12, 10, 11};

    for (int channel=0; channel<7; channel++){
        // std::string channel_name = Form("%i",channel);
        TCut xCut = "x_dut[0]>-1 && x_dut[0]<1";
        TCut yCut = "y_dut[0]>9.5 && y_dut[0]<12.5";
        TCut ampCut = Form("amp[%i]>%i",channel,amp_low_cut[channel]);

        auto htitleamp = Form("amp[%i];amp[%i] [mV];Counts",channel,channel);
        TH1F *histamp = new TH1F(Form("amp%i",channel),htitleamp,100,0,400);
        auto htitlexy = Form("Hits in channel %i with amp[%i]>%i;x_dut [mm];y_dut [mm]",channel,channel,amp_low_cut[channel]);
        TH2F *histxy = new TH2F(Form("ampxy%i",channel),htitlexy,100,-1,1,10,9.5,12.5);

        chain->Draw(Form("amp[%i]>>amp%i",channel,channel),Form("amp[%i]>0",channel));
        chain->Draw(Form("y_dut[0]:x_dut[0]>>ampxy%i",channel),xCut+yCut+ampCut,"COLZ");
    }
}
