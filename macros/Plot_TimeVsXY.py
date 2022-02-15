from ROOT import TFile,TTree,TCanvas,TH1F,TH2F,TLatex,TMath,TEfficiency,TGraphAsymmErrors,TLegend,gROOT,gStyle, kWhite
import os
import optparse
import myStyle

gROOT.SetBatch( True )
# gStyle.SetOptFit(1011)
gStyle.SetOptStat(0)

## Defining Style
myStyle.ForceStyle()
# gStyle.SetTitleYOffset(1.1)

# Construct the argument parser
parser = optparse.OptionParser("usage: %prog [options]\n")
parser.add_option('-f', dest='file', default = "Output_LaserMultiSnsr.root", help="File name (+ path from ../)")
# parser.add_option('-n','--nocorrection', action="store_true", dest='nocorrection', default = False, help="Apply [default] or not factor correction to amp")
# parser.add_option('-b','--biasvolt', dest='biasvolt', default = 220, help="Bias Voltage value in [V]")
options, args = parser.parse_args()

file = options.file
# nocorrection = options.nocorrection
# bias = options.biasvolt

inputfile = TFile("../"+file,"READ")

color = [416+2, 432+2, 600, 880, 632, 400+2]

canvas = TCanvas("cv", "cv", 1000, 800)

htmp = inputfile.Get("hTimeVsXY_Ch0").Clone("htmp") # hTimeVsXY_Ch%i
ymin = htmp.GetYaxis().GetXmin() - 0.25
ymax = htmp.GetYaxis().GetXmax()
nxbins = htmp.GetXaxis().GetNbins()
xmin = htmp.GetXaxis().GetXmin() - 0.50
xmax = htmp.GetXaxis().GetXmax()
nybins = htmp.GetYaxis().GetNbins()
htmp.Delete()

# if ("MultiSnsr" in file):
#     amp_max = 180.
# elif ("EIC" in file):
#     amp_max = 250.

## Time Vs Y, per X=constant bin
for iX in range(nxbins):
    # htmp.SetTitle("Amplitude"+corr_title+" Vs Y_laser, X["+str(iX)+"];y_laser [mm];amp"+corr_title+" [mV]")
    htmp = TH1F("htmp", "#DeltaTime Vs Y_laser, X["+str(iX)+"];y_laser [mm];#Deltat[ns]", 1, ymin, ymax)
    htmp.GetYaxis().SetRangeUser(54.5, 57.5) # Check if these values can be retrieved with ~ htmp.GetZaxis().GetXmax()
    htmp.Draw("AXIS")
    # list_hTimeVsY = []
    legend = TLegend(2*myStyle.GetMargin()+0.02,1-myStyle.GetMargin()-0.02-0.1,1-myStyle.GetMargin()-0.02,1-myStyle.GetMargin()-0.02)
    legend.SetNColumns(3)
    legend.SetTextFont(myStyle.GetFont())
    legend.SetTextSize(myStyle.GetSize())
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.SetFillColor(kWhite) 
    for iCh in range(6):
        hTimeVsY = inputfile.Get("hTimeVsXY_Ch"+str(iCh)).ProjectionY("hTimeVsY_Ch"+str(iCh),iX+1,iX+1)
        hTimeVsY.SetMarkerStyle(21)
        hTimeVsY.SetLineColor(color[iCh])
        hTimeVsY.SetLineWidth(3)
        legend.AddEntry(hTimeVsY, "Channel "+str(iCh))
        # list_hTimeVsY[iCh].GetYaxis().SetRangeUser(0.,220.)
        hTimeVsY.Draw("SAME HIST LP")
        hTimeVsY.Delete() # ???
    legend.Draw()
    canvas.SaveAs("TimeVsY_X"+str(iX)+".gif")
    canvas.Clear()
    htmp.Delete()
    # canvas.Delete()
