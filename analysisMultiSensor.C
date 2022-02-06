
void analysisMultiSensor(){
    //// Input
    // TChain *chain = new TChain("pulse");
    // chain->Add("run_scope*.root");

    //// Output
    TFile *output = TFile::Open("MultiSensor.root","RECREATE");

    // int Nentries = chain->GetEntries();
    // std::cout << "Total number of entries: " << Nentries << std::endl;

    // Definition of branches and leaves
    // chain->SetBranchStatus("*",0);
    // chain->SetBranchStatus("amp",1);
    // chain->SetBranchStatus("x_laser",1);
    // chain->SetBranchStatus("y_laser",1);

    // float amp[8], x_laser, y_laser;

    // chain->SetBranchAddress("amp",amp);
    // chain->SetBranchAddress("x_laser",x_laser);
    // chain->SetBranchAddress("y_laser",y_laser);

    // // Amp threshold value and position of wires
    // int amp_Low_Cut[8] = {60, 60, 60, 70, 70, 70, 70, 70};
    // int amp_High_Cut[8] = {220, 200, 200, 200, 180, 180, 180, 350};

    // float xy_Coord[4] = {-1.1, 1.5, 9.5, 12.2}; // {x_min, x_max, y_min, y_max}
    // float ch_Limits[6][4] = {{0.29, 0.41, 10.0, 11.6}, {0.20, 0.30, 10, 11.6}, {0.10, 0.20, 10, 11.6},
    //                         {0.00, 0.10, 10.0, 11.6}, {-0.10, -0.01, 10, 11.6}, {-0.21, -0.11, 10, 11.6}};
    // float timeCorrTerm[6] = {10.5553183648148, 10.6590122524753, 10.6470996902006,
    //                         10.6075997712141, 10.6432860123283, 10.6622858488865};

    // Create histograms
    std::vector<TH1F*> hamp_Vec;
    std::vector<TH2F*> hampY_Vec;

    std::vector<float> x_range = {36.5, 37.0, 37.5, 38.0, 38.5, 39.0, 39.5};
    std::vector<float> y_range = {14.00, 14.25, 14.50, 14.75, 15.00, 15.25, 15.50, 15.75, 16.00, 16.25, 16.50, 16.75, 17.00}; // +0.05
    std::vector<float> y_range_REAL = {14.05, 14.30, 14.55, 14.80, 15.05, 15.30, 15.55, 15.80, 16.05, 16.30, 16.55, 16.80, 17.05};

    // D1. Make amp[per channel] TH1F for each .root file
    // D2. Fit each distribution with a 'gaus' in the range (mean - 1.2*std_dev, mean + 1.2*std_dev)
    // D3. Get the mean value of the fit (This should give the top values).
    // R4. Collect those values and present them as TGraphs, one for each x (x_range.size() in total)

    // std::vector<std::vector<std::vector<float>>> amp_peak(x_range.size(), y_range);
    const int x_size = x_range.size();
    const int y_size = y_range.size();
    float amp_peak[x_size][y_size][6];

    for (int run=9183; run<9272; run++){
        TFile *input_tmp = TFile::Open("run_scope"+std::to_string(run)+".root","READ");
        TTree *tree_tmp = (TTree*)input_tmp->Get("pulse");

        tree_tmp->SetBranchStatus("*",0);
        tree_tmp->SetBranchStatus("amp",1);
        tree_tmp->SetBranchStatus("x_laser",1);
        tree_tmp->SetBranchStatus("y_laser",1);

        float amp[8], x_laser, y_laser;

        tree_tmp->SetBranchAddress("amp",amp);
        tree_tmp->SetBranchAddress("x_laser",x_laser);
        tree_tmp->SetBranchAddress("y_laser",y_laser);

        std::vector<TH1F*> hamp_tmp_Vec(6);
        for (int h=0; h<hamp_tmp_Vec.size(); h++){
            TH1F *hamp_tmp = new TH1F(Form("hamp_tmp%i",h), Form("amp[%i]",h), 220, 0, 220);
            hamp_tmp_Vec.push_back(hamp_tmp);
        }

        float x_y[2] = {0.0,0.0};

        int Nentries_tmp = tree_tmp->GetEntries();
        for (int i=0; i<Nentries_tmp; i++){
            tree_tmp->GetEntry(i);
            if (i==0){
                x_y[0] = x_laser;
                x_y[1] = y_laser - 0.05; // 0.05 added for an error when transforming float into int (and viceversa)
            }

            for (int ch=0; ch<6; ch++){
                hamp_tmp_Vec[ch]->Fill(amp[ch]);
            }
        }

        for (int ch=0; ch<6; ch++){
            hamp_tmp_Vec[ch]->Fit("gaus","","", (hamp_tmp_Vec[ch]->GetMean()) - 1.2*(hamp_tmp_Vec[ch]->GetStdDev()),
                                                (hamp_tmp_Vec[ch]->GetMean()) + 1.2*(hamp_tmp_Vec[ch]->GetStdDev()));
            TF1 *fit_tmp = hamp_tmp_Vec[ch]->GetFunction("gaus");
            float mean = fit_tmp->GetParameter(1);
            // float sdev = fit_tmp->GetParameter(2);
            amp_peak[(x_y[0]-36.5)/0.5][(x_y[1]-14)/0.25][ch] = mean;
        }

        input_tmp->Close();
    }

    // Create TGraph

    for (int iX=0; iX<x_size; iX++){

        std::vector<TGraph*> graph_YvsCh(6); 

        // Create one TGraph for each channel of each y
        for (int ich=0; ich<6; ich++){
            TGraph *graph_tmp = new TGraph(y_size);
            for (int iY=0; iY<y_size; iY++){
                graph_tmp->AddPoint(y_range_REAL[iY], amp_peak[iX][iY][ich]);
            }

            graph_YvsCh[ich] = graph_tmp.Clone(Form("YvsCh%i_X%i", ich, iX));
            graph_YvsCh[ich].Write();
        }

        graph_YvsCh.clear();
    }
    
    // Save and close
    output->Write();
    output->Close();
}
