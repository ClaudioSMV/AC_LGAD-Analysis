
void analysis_sensor2022(TString relative_path_input = "./", TString sensor_name = ""){

    gROOT->SetBatch(true);

    //// Input
    TChain *chain = new TChain("pulse");
    chain->Add(relative_path_input+"run_scope*.root");

    //// Output
    TFile *output = TFile::Open("Output_"+sensor_name+".root","RECREATE");

    chain->SetBranchStatus("*",0);
    chain->SetBranchStatus("amp",1);
    chain->SetBranchStatus("x_laser",1);
    chain->SetBranchStatus("y_laser",1);
    chain->SetBranchStatus("LP2_20",1);

    std::vector<float> x_range;
    std::vector<float> x_range_REAL;
    std::vector<float> y_range;
    std::vector<float> y_range_REAL;
    float amp_corr_val=0;

    // // Sensor: LaserMultiSnsr
    if (sensor_name=="LaserMultiSnsr"){
        x_range = {36.5, 37.0, 37.5, 38.0, 38.5, 39.0, 39.5};
        x_range_REAL = {36.5, 37.0, 37.5, 38.0, 38.5, 39.0, 39.5};
        y_range = {14.00, 14.25, 14.50, 14.75, 15.00, 15.25, 15.50, 15.75, 16.00, 16.25, 16.50, 16.75, 17.00}; // this = real - 0.05
        y_range_REAL = {14.05, 14.30, 14.55, 14.80, 15.05, 15.30, 15.55, 15.80, 16.05, 16.30, 16.55, 16.80, 17.05};
        amp_corr_val = 120.;
    }

    // // Sensor: EIC1p0CM
    else if (sensor_name=="EIC1p0CM"){
        x_range = {31.5, 32.0, 32.5, 33.0, 33.5, 34.0, 34.5, 35.0, 35.5, 36.0, 36.5, 37.0, 37.5, 38.0, 38.5, 39.0, 39.5, 40.0, 40.5, 41.0, 41.5, 42.0};
        x_range_REAL = {31.6, 32.1, 32.6, 33.1, 33.6, 34.1, 34.6, 35.1, 35.6, 36.1, 36.6, 37.1, 37.6, 38.1, 38.6, 39.1, 39.6, 40.1, 40.6, 41.1, 41.6, 42.1};
        y_range = {12.50, 12.75, 13.00, 13.25, 13.50, 13.75, 14.00, 14.25, 14.50, 14.75, 15.00, 15.25, 15.50, 15.75, 16.00, 16.25, 16.50, 16.75, 17.00, 17.25, 17.50};
        y_range_REAL = {12.60, 12.85, 13.10, 13.35, 13.60, 13.85, 14.10, 14.35, 14.60, 14.85, 15.10, 15.35, 15.60, 15.85, 16.10, 16.35, 16.60, 16.85, 17.10, 17.35, 17.60};
        amp_corr_val = 67.;
    }

    // // Test with run_scope9183.root and run_scope9184.root
    else if (sensor_name=="test"){
        x_range = {38.0};
        x_range_REAL = {38.0};
        y_range = {14.25, 14.50}; // Real=this+0.05
        y_range_REAL = {14.30, 14.55};
	amp_corr_val = 100.;
    }

    const int n_channels = 6 + 2;
    const int n_strips = 6;
    const int x_size = x_range.size();
    const int y_size = y_range.size();
    const float t_min = 54.5; // in [ns]
    const float t_max = 56.5; // in [ns]

    // // // // //

    float amp[8], x_laser, y_laser, LP2_20[8];

    chain->SetBranchAddress("amp", amp);
    chain->SetBranchAddress("x_laser", &x_laser);
    chain->SetBranchAddress("y_laser", &y_laser);
    chain->SetBranchAddress("LP2_20", LP2_20);

    // // I: Creating 'raw' histograms
    std::vector<TH1F*> hAmp_Vec;
    std::vector<TH1F*> hTime_Vec;

    for (int iX=0; iX<x_size; iX++){
        for (int iY=0; iY<y_size; iY++){
            for (int ich=0; ich<n_channels; ich++){
                TH1F *hamp_tmp = new TH1F(Form("hAmp_X%iY%iCh%i",iX,iY,ich), Form("Amp, X = %.2f, Y = %.2f;amp[%i];Counts",x_range[iX],y_range[iY],ich), 220, 0, 220);
                hAmp_Vec.push_back(hamp_tmp);
                
                TH1F *htime_tmp = new TH1F(Form("hTime_X%iY%iCh%i",iX,iY,ich), Form("Time Delta, X = %.2f, Y = %.2f;LP2_20[%i] - LP2_20[6] [ns];Counts",x_range[iX],y_range[iY],ich), 400, t_min, t_max);
                hTime_Vec.push_back(htime_tmp);
            }
        }
    }
    // // F: 'Raw' histograms created!

    // // I: Running over tree-entries
    int Nentries = chain->GetEntries();

    int perc = 1;
    for (int i=0; i<Nentries; i++){
        chain->GetEntry(i);

        if (i > perc*Nentries/4.){
            std::cout << "\t" << perc*25 << "%" << " of data processed." << std::endl;
            perc++;
        }

        int x_pos = (int) round((x_laser - x_range_REAL[0])/0.5);
        int y_pos = (int) round((y_laser - y_range_REAL[0])/0.25);

        for (int ich=0; ich<n_channels; ich++){
            if (amp[ich]>0){
                hAmp_Vec[ich + y_pos*n_channels + x_pos*y_size*n_channels]->Fill(amp[ich]);
            }

            if (LP2_20[ich]!=0 && LP2_20[6]!=0){
                hTime_Vec[ich + y_pos*n_channels + x_pos*y_size*n_channels]->Fill((LP2_20[ich] - LP2_20[6])*1.e9); // if (ich!=6) 
                // else hTime_Vec[ich + y_pos*n_channels + x_pos*y_size*n_channels]->Fill(LP2_20[6]);
            }
        }
    }

    std::cout << "\t" << 100 << "%" << " of data processed. DONE!" << std::endl;
    // // F: Tree-entries read!

    std::cout << "Creating histograms and TGraphs..." << std::endl;

    std::vector<TH2F*> hAmpVsXY_Vec;
    std::vector<TH2F*> hAmpVsXY_Corr_Vec;
    std::vector<TH2F*> hTimeVsXY_Vec;

    double contour[20];
    for (int i=0; i<20; i++){
        contour[i] = t_min + (t_max-t_min)*i/19;
    }

    for (int iCh=0; iCh<n_channels; iCh++){
        TH2F *hAmpVsXY = new TH2F(Form("hAmpVsXY_Ch%i",iCh), Form("Amplitude Ch %i;x_laser [mm];y_laser [mm]",iCh), x_size, x_range[0], x_range[x_size-1] + 0.5,
                                  y_size, y_range[0], y_range[y_size-1] + 0.25);
        hAmpVsXY_Vec.push_back(hAmpVsXY);

        TH2F *hAmpVsXY_Corr = new TH2F(Form("hAmpVsXY_Corr_Ch%i",iCh), Form("Amplitude corrected Ch %i;x_laser [mm];y_laser [mm]",iCh), x_size, x_range[0], x_range[x_size-1] + 0.5,
                                  y_size, y_range[0], y_range[y_size-1] + 0.25);
        hAmpVsXY_Corr_Vec.push_back(hAmpVsXY_Corr);

        TH2F *hTimeVsXY = new TH2F(Form("hTimeVsXY_Ch%i",iCh), Form("#DeltaTime Ch %i;x_laser [mm];y_laser [mm]",iCh), x_size, x_range[0], x_range[x_size-1] + 0.5,
                                  y_size, y_range[0], y_range[y_size-1] + 0.25);
        hTimeVsXY->SetContour(20, contour);
        hTimeVsXY->GetZaxis()->SetRangeUser(contour[0], contour[19]);
        hTimeVsXY_Vec.push_back(hTimeVsXY);
    }

    std::vector<TGraph*> graphAmpVsX_YConst_Vec;
    std::vector<TGraph*> graphAmpCorrVsX_YConst_Vec;

    for (int iY=0; iY<y_size; iY++){
        for (int iCh=0; iCh<n_channels; iCh++){
            TGraph *graphAmpVsX_YConst_tmp = new TGraph(x_size);
            graphAmpVsX_YConst_tmp->SetName(Form("Amp%iVsXLaser_Y%i",iCh,iY));
            graphAmpVsX_YConst_tmp->SetTitle(Form("Amp, Y = %.2f;x_laser [mm];Mean amp[%i]",y_range[iY],iCh));
            graphAmpVsX_YConst_Vec.push_back(graphAmpVsX_YConst_tmp); // Graph position = ich + iY*n_channels

            TGraph *graphAmpCorrVsX_YConst_tmp = new TGraph(x_size);
            graphAmpCorrVsX_YConst_tmp->SetName(Form("Amp%iVsXLaser_Corr_Y%i",iCh,iY));
            graphAmpCorrVsX_YConst_tmp->SetTitle(Form("Amp corrected, Y = %.2f;x_laser [mm];Mean amp[%i]",y_range[iY],iCh));
            graphAmpCorrVsX_YConst_Vec.push_back(graphAmpCorrVsX_YConst_tmp); // Graph position = ich + iY*n_channels            
        }
    }

    // std::vector<float> max_pair_xcst[2]; // Two vectors with entries [0] = y_max; [1] = amp_max
    // std::vector<float> max_pair_ycst[2]; // Two vectors with entries [0] = x_max; [1] = amp_max

    int ch_laser = 7;
    int cut_time_entries = 80;

    for (int jX=0; jX<x_size; jX++){
        for (int jCh=0; jCh<n_channels; jCh++){
            std::vector<float> amp_value(y_size);
            std::vector<float> new_amp_value(y_size);
            std::vector<float> time_value(y_size);

            // float y_max=0, amp_max=0; 

            for (int jY=0; jY<y_size; jY++){
                float mean = hAmp_Vec[jCh + jY*n_channels + jX*y_size*n_channels]->GetMean();
                amp_value[jY] = mean;

                float amp_laser = hAmp_Vec[ch_laser + jY*n_channels + jX*y_size*n_channels]->GetMean();

                if (amp_laser>0.) new_amp_value[jY] = amp_corr_val*mean/amp_laser;
                else new_amp_value[jY] = 0;

                float mean_time = 0;
                if (hTime_Vec[jCh + jY*n_channels + jX*y_size*n_channels]->GetEntries() > cut_time_entries) mean_time = hTime_Vec[jCh + jY*n_channels + jX*y_size*n_channels]->GetMean();
                time_value[jY] = mean_time;

                // if (new_amp_value[jY] > amp_max){
                //     amp_max = new_amp_value[jY];
                //     y_max = y_range[jY];
                // }
                
                // std::cout << "X="<< jX << "; CH=" << jCh << "; Y=" << jY << "; Amp="<< printf("%.2f",amp_laser);
                // std::cout << "; AmpCorr=" << printf("%.2f",100*mean/amp_laser) << "; AmpLaser=" << printf("%.2f",amp_laser) << std::endl;

                if (jCh!=6) hAmpVsXY_Vec[jCh]->Fill(x_range[jX], y_range[jY], amp_value[jY]);
                else if (jX==0 && jY==0) hAmpVsXY_Vec[jCh]->Delete();

                if (jCh!=6) hAmpVsXY_Corr_Vec[jCh]->Fill(x_range[jX], y_range[jY], new_amp_value[jY]);
                else if (jX==0 && jY==0) hAmpVsXY_Corr_Vec[jCh]->Delete();

                if (jCh<6) hTimeVsXY_Vec[jCh]->Fill(x_range[jX], y_range[jY], time_value[jY]);
                else if (jX==0 && jY==0) hTimeVsXY_Vec[jCh]->Delete();

                hAmp_Vec[jCh + jY*n_channels + jX*y_size*n_channels]->Delete();
                // hTime_Vec[jCh + jY*n_channels + jX*y_size*n_channels]->Delete();
                graphAmpVsX_YConst_Vec[jCh + jY*n_channels]->SetPoint(jX, x_range[jX], amp_value[jY]);
                graphAmpCorrVsX_YConst_Vec[jCh + jY*n_channels]->SetPoint(jX, x_range[jX], new_amp_value[jY]);
            }

            if (jCh<n_strips){
                TGraph *graphAmpVsY_XConst_tmp = new TGraph(y_size, &y_range[0], &amp_value[0]);
                graphAmpVsY_XConst_tmp->SetName(Form("Amp%iVsYLaser_X%i",jCh,jX));
                graphAmpVsY_XConst_tmp->SetTitle(Form("Amp, X = %.2f;y_laser [mm];Mean amp[%i]",x_range[jX],jCh));
                graphAmpVsY_XConst_tmp->Write();
                graphAmpVsY_XConst_tmp->Delete();

                TGraph *graphAmpCorrVsY_XConst_tmp = new TGraph(y_size, &y_range[0], &new_amp_value[0]);
                graphAmpCorrVsY_XConst_tmp->SetName(Form("Amp%iVsYLaser_Corr_X%i",jCh,jX));
                graphAmpCorrVsY_XConst_tmp->SetTitle(Form("Amp corrected, X = %.2f;y_laser [mm];Mean amp[%i]",x_range[jX],jCh));
                graphAmpCorrVsY_XConst_tmp->Write();
                graphAmpCorrVsY_XConst_tmp->Delete();
            }

            amp_value.clear();
            new_amp_value.clear();
        }
    }

    for (int iY=0; iY<y_size; iY++){
        for (int iCh=0; iCh<n_channels; iCh++){
            if (iCh<n_strips) graphAmpVsX_YConst_Vec[iCh + iY*n_channels]->Write();
            graphAmpVsX_YConst_Vec[iCh + iY*n_channels]->Delete();

            if (iCh<n_strips) graphAmpCorrVsX_YConst_Vec[iCh + iY*n_channels]->Write();
            graphAmpCorrVsX_YConst_Vec[iCh + iY*n_channels]->Delete();
        }
    }

    std::cout << "Histograms and TGraphs already created!" << std::endl;

    // Save and close
    output->Write();
    output->Close();
}
