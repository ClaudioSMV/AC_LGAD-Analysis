
void analysisMultiSensor(TString relative_path = "./"){

    gROOT->SetBatch(true);

    //// Input
    TChain *chain = new TChain("pulse");
    chain->Add(relative_path+"run_scope*.root");

    //// Output
    TFile *output = TFile::Open("MultiSensor.root","RECREATE");

    chain->SetBranchStatus("*",0);
    chain->SetBranchStatus("amp",1);
    chain->SetBranchStatus("x_laser",1);
    chain->SetBranchStatus("y_laser",1);
    chain->SetBranchStatus("LP2_20",1);

    std::vector<float> x_range = {36.5, 37.0, 37.5, 38.0, 38.5, 39.0, 39.5};
    std::vector<float> y_range = {14.00, 14.25, 14.50, 14.75, 15.00, 15.25, 15.50, 15.75, 16.00, 16.25, 16.50, 16.75, 17.00}; // +0.05
    std::vector<float> y_range_REAL = {14.05, 14.30, 14.55, 14.80, 15.05, 15.30, 15.55, 15.80, 16.05, 16.30, 16.55, 16.80, 17.05};

    // // Test with run_scope9183.root
    // std::vector<float> x_range = {38.0};
    // std::vector<float> y_range = {14.25}; // Real=this+0.05
    // std::vector<float> y_range_REAL = {14.30};

    const int n_channels = 6 + 2;
    const int x_size = x_range.size();
    const int y_size = y_range.size();

    // // // // //

    float amp[8], x_laser, y_laser, LP2_20[8];

    chain->SetBranchAddress("amp", amp);
    chain->SetBranchAddress("x_laser", &x_laser);
    chain->SetBranchAddress("y_laser", &y_laser);
    chain->SetBranchAddress("LP2_20", LP2_20);

    std::vector<TH1F*> hAmp_Vec;
    std::vector<TH1F*> hTime_Vec;

    for (int iX=0; iX<x_size; iX++){
        for (int iY=0; iY<y_size; iY++){
            for (int ich=0; ich<n_channels; ich++){
                TH1F *hamp_tmp = new TH1F(Form("hAmp_X%iY%iCh%i",iX,iY,ich), Form("Amp, X = %.2f, Y = %.2f;amp[%i];Counts",x_range[iX],y_range[iY],ich), 220, 0, 220);
                hAmp_Vec.push_back(hamp_tmp);
                
                TH1F *htime_tmp = new TH1F(Form("hTime_X%iY%iCh%i",iX,iY,ich), Form("Time Delta, X = %.2f, Y = %.2f;LP2_20[%i] - LP2_20[6];Counts",x_range[iX],y_range[iY],ich), 200, 4.e-8, 6.e-8);
                hTime_Vec.push_back(htime_tmp);
            }
        }
    }

    // // Running over tree-entries
    int Nentries = chain->GetEntries();

    int perc = 1;
    for (int i=0; i<Nentries; i++){
        chain->GetEntry(i);

        if (i > perc*Nentries/4.){
            std::cout << "\t" << perc*25 << "%" << " of data processed." << std::endl;
            perc++;
        }

        int x_pos = (int) round((x_laser - 36.5)/0.5);
        int y_pos = (int) round((y_laser - 14.05)/0.25);

        for (int ich=0; ich<n_channels; ich++){
            if (amp[ich]>0){
                hAmp_Vec[ich + y_pos*n_channels + x_pos*y_size*n_channels]->Fill(amp[ich]);
            }

            if (LP2_20[ich]!=0 && LP2_20[6]!=0){
                hTime_Vec[ich + y_pos*n_channels + x_pos*y_size*n_channels]->Fill(LP2_20[ich] - LP2_20[6]);
            }
        }
    }

    std::cout << "\t" << 100 << "%" << " of data processed. DONE!" << std::endl;
    std::cout << "Creating histograms and TGraphs..." << std::endl;

    std::vector<TH2F*> hAmpVsXY_Vec;
    std::vector<TH2F*> hAmpVsXY_Corr_Vec;
    for (int iCh=0; iCh<n_channels; iCh++){
        TH2F *hAmpVsXY = new TH2F(Form("hAmpVsXY_Ch%i",iCh), Form("Amplitude Ch %i;x_laser [mm];y_laser [mm]",iCh), x_size, x_range[0], x_range[x_size-1] + 0.5,
                                  y_size, y_range[0], y_range[y_size-1] + 0.25);
        hAmpVsXY_Vec.push_back(hAmpVsXY);

        TH2F *hAmpVsXY_Corr = new TH2F(Form("hAmpVsXY_Corr_Ch%i",iCh), Form("Amplitude corrected Ch %i;x_laser [mm];y_laser [mm]",iCh), x_size, x_range[0], x_range[x_size-1] + 0.5,
                                  y_size, y_range[0], y_range[y_size-1] + 0.25);
        hAmpVsXY_Corr_Vec.push_back(hAmpVsXY_Corr);
    }

    std::vector<TGraph*> graph_y_const_Vec;

    for (int iY=0; iY<y_size; iY++){
        for (int iCh=0; iCh<n_channels; iCh++){
            TGraph *graph_y_const_tmp = new TGraph(x_size);
            graph_y_const_tmp->SetName(Form("Amp%iVsXLaser_Y%i",iCh,iY));
            graph_y_const_tmp->SetTitle(Form("Amp, Y = %.2f;x_laser [mm];Mean amp[%i]",y_range[iY],iCh));
            graph_y_const_Vec.push_back(graph_y_const_tmp); // Graph position = ich + iY*n_channels
        }
    }

    // std::vector<float> max_pair_xcst[2]; // Two vectors with entries [0] = y_max; [1] = amp_max
    // std::vector<float> max_pair_ycst[2]; // Two vectors with entries [0] = x_max; [1] = amp_max

    int ch_laser = 7; // CHECK: Amp[6] ~ 1450. Is that good?
    for (int jX=0; jX<x_size; jX++){
        for (int jCh=0; jCh<n_channels; jCh++){
            std::vector<float> amp_value(y_size);
            std::vector<float> new_amp_value(y_size);

            // float y_max=0, amp_max=0; 

            for (int jY=0; jY<y_size; jY++){
                float mean = hAmp_Vec[jCh + jY*n_channels + jX*y_size*n_channels]->GetMean();
                amp_value[jY] = mean;

                float amp_laser = hAmp_Vec[ch_laser + jY*n_channels + jX*y_size*n_channels]->GetMean();
                new_amp_value[jY] = 100*mean/amp_laser;

                // if (new_amp_value[jY] > amp_max){
                //     amp_max = new_amp_value[jY];
                //     y_max = y_range[jY];
                // }
                
                // std::cout << "X="<< jX << "; CH=" << jCh << "; Y=" << jY << "; Amp="<< printf("%.2f",amp_laser);
                // std::cout << "; AmpCorr=" << printf("%.2f",100*mean/amp_laser) << "; AmpLaser=" << printf("%.2f",amp_laser) << std::endl;

                hAmpVsXY_Vec[jCh]->Fill(x_range[jX], y_range[jY], mean);

                hAmpVsXY_Corr_Vec[jCh]->Fill(x_range[jX], y_range[jY], 100*mean/amp_laser);

                hAmp_Vec[jCh + jY*n_channels + jX*y_size*n_channels]->Delete();
                graph_y_const_Vec[jCh + jY*n_channels]->SetPoint(jX, x_range[jX], new_amp_value[jY]);
            }
            
            TGraph *graph_tmp = new TGraph(y_size, &y_range[0], &amp_value[0]);
            graph_tmp->SetName(Form("Amp%iVsYLaser_X%i",jCh,jX));
            graph_tmp->SetTitle(Form("Amp, X = %.2f;y_laser [mm];Mean amp[%i]",x_range[jX],jCh));
            graph_tmp->Write();
            graph_tmp->Delete();

            TGraph *graph_Corr_tmp = new TGraph(y_size, &y_range[0], &new_amp_value[0]);
            graph_Corr_tmp->SetName(Form("Amp%iVsYLaser_Corr_X%i",jCh,jX));
            graph_Corr_tmp->SetTitle(Form("Amp corrected, X = %.2f;y_laser [mm];Mean amp[%i]",x_range[jX],jCh));
            graph_Corr_tmp->Write();
            graph_Corr_tmp->Delete();

            amp_value.clear();
            new_amp_value.clear();
        }
    }

    for (int i=0; i<y_size*n_channels; i++){
        graph_y_const_Vec[i]->Write();
        graph_y_const_Vec[i]->Delete();
    }

    std::cout << "Histograms and TGraphs (X const) already created!" << std::endl;

    // Save and close
    output->Write();
    output->Close();
}
