
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

    // // Amp threshold value and position of wires
    // int amp_Low_Cut[8] = {60, 60, 60, 70, 70, 70, 70, 70};
    // int amp_High_Cut[8] = {220, 200, 200, 200, 180, 180, 180, 350};

    // float xy_Coord[4] = {-1.1, 1.5, 9.5, 12.2}; // {x_min, x_max, y_min, y_max}
    // float ch_Limits[6][4] = {{0.29, 0.41, 10.0, 11.6}, {0.20, 0.30, 10, 11.6}, {0.10, 0.20, 10, 11.6},
    //                         {0.00, 0.10, 10.0, 11.6}, {-0.10, -0.01, 10, 11.6}, {-0.21, -0.11, 10, 11.6}};
    // float timeCorrTerm[6] = {10.5553183648148, 10.6590122524753, 10.6470996902006,
    //                         10.6075997712141, 10.6432860123283, 10.6622858488865};

    std::vector<float> x_range = {36.5, 37.0, 37.5, 38.0, 38.5, 39.0, 39.5};
    std::vector<float> y_range = {14.00, 14.25, 14.50, 14.75, 15.00, 15.25, 15.50, 15.75, 16.00, 16.25, 16.50, 16.75, 17.00}; // +0.05
    std::vector<float> y_range_REAL = {14.05, 14.30, 14.55, 14.80, 15.05, 15.30, 15.55, 15.80, 16.05, 16.30, 16.55, 16.80, 17.05};

    // // Test with run_scope9183.root
    // std::vector<float> x_range = {38.0};
    // std::vector<float> y_range = {14.25}; // Real=this+0.05
    // std::vector<float> y_range_REAL = {14.30};

    const int x_size = x_range.size();
    const int y_size = y_range.size();

    // for (int iX=0; iX<x_size; iX++){
    //     for (int ich=0; ich<6; ich++){
    //         std::vector<float> amp_value(y_size);

    //         // for (int iY=0; iY<y_size; iY++){
    //         //     TH1F *hamp_tmp = new TH1F("hamp_tmp", "Amp", 220, 0, 220);
    //         //     chain->Draw(Form("amp[%i]>>hamp_tmp", ich), Form("amp[%i]>=0 && x_laser>0.999*%f && x_laser<1.001*%f && y_laser>0.999*%f && y_laser<1.001*%f",
    //         //                                                      ich, x_range[iX], x_range[iX], y_range_REAL[iY], y_range_REAL[iY]));
    //         //     float mean = hamp_tmp->GetMean();

    //         //     // std::cout << "Mean strip " << ich << ", y[" << iY << "]: " << mean << std::endl;
    //         //     // std::cout << Form("x_laser %f, y_laser %f ", x_range[iX], y_range_REAL[iY]) << std::endl;

    //         //     amp_value[iY] = mean;
    //         //     hamp_tmp->Delete();
    //         // }

    //         TGraph *graph_tmp = new TGraph(y_size, &y_range_REAL[0], &amp_value[0]);
    //         graph_tmp->SetName(Form("Amp%iVsYLaser_X%i",ich,iX));
    //         graph_tmp->Write();

    //         amp_value.clear();
    //     }
    // }

    // // // // //

    float amp[8], x_laser, y_laser;

    chain->SetBranchAddress("amp", amp);
    chain->SetBranchAddress("x_laser", &x_laser);
    chain->SetBranchAddress("y_laser", &y_laser);

    std::vector<TH1F*> hAmp_Vec;

    for (int iX=0; iX<x_size; iX++){
        for (int iY=0; iY<y_size; iY++){
            for (int ich=0; ich<6; ich++){
                TH1F *hamp_tmp = new TH1F(Form("hAmp_X%iY%iCh%i",iX,iY,ich), Form("Amp, X = %.2f, Y = %.2f;amp[%i];Counts",x_range[iX],y_range_REAL[iY],ich), 220, 0, 220);
                hAmp_Vec.push_back(hamp_tmp); // Histogram position = ich + iY*6 + iX*y_size*6
            }
        }
    }

    int Nentries = chain->GetEntries();

    for (int i=0; i<Nentries; i++){
        chain->GetEntry(i);

        int x_pos = (int) round((x_laser - 36.5)/0.5);
        int y_pos = (int) round((y_laser - 14.05)/0.25);

        for (int ich=0; ich<6; ich++){
            if (amp[ich]>0){
                hAmp_Vec[ich + y_pos*6 + x_pos*y_size*6]->Fill(amp[ich]);
            }
        }
    }

    for (int jX=0; jX<x_size; jX++){
        for (int jCh=0; jCh<6; jCh++){
            std::vector<float> amp_value(y_size);

            for (int jY=0; jY<y_size; jY++){
                float mean = hAmp_Vec[jCh + jY*6 + jX*y_size*6]->GetMean();
                amp_value[jY] = mean;

                // hAmp_Vec[jCh + jY*6 + jX*y_size*6]->Delete();
            }
            
            TGraph *graph_tmp = new TGraph(y_size, &y_range_REAL[0], &amp_value[0]);
            graph_tmp->SetName(Form("Amp%iVsYLaser_X%i",jCh,jX));
            graph_tmp->Write();
            graph_tmp->Delete();

            amp_value.clear();
        }
    }

    std::cout << "TGraphs created!" << std::endl;

    // Save and close
    output->Write();
    output->Close();
}
