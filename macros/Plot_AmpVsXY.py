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
parser.add_option('-n','--nocorrection', action="store_true", dest='nocorrection', default = False, help="Apply [default] or not factor correction to amp")
# parser.add_option('-b','--biasvolt', dest='biasvolt', default = 220, help="Bias Voltage value in [V]")
options, args = parser.parse_args()

file = options.file
nocorrection = options.nocorrection
# bias = options.biasvolt

if not nocorrection:
    corr_title = "_Corr"
else:
    corr_title = ""

inputfile = TFile("../"+file,"READ")

color = [416+2, 432+2, 600, 880, 632, 400+2]

canvas = TCanvas("cv", "cv", 1000, 800)

htmp = inputfile.Get("hAmpVsXY_Ch0").Clone("htmp")
ymin = htmp.GetYaxis().GetXmin() - 0.25
ymax = htmp.GetYaxis().GetXmax()
nxbins = htmp.GetXaxis().GetNbins()
htmp.Delete()

if ("MultiSnsr" in file):
    amp_max = 180.
elif ("EIC" in file):
    amp_max = 250.

for iX in range(nxbins):
    # htmp.SetTitle("Amplitude"+corr_title+" Vs Y_laser, X["+str(iX)+"];y_laser [mm];amp"+corr_title+" [mV]")
    htmp = TH1F("htmp", "Amplitude"+corr_title+" Vs Y_laser, X["+str(iX)+"];y_laser [mm];amp"+corr_title+" [mV]", 1, ymin, ymax)
    htmp.GetYaxis().SetRangeUser(0., amp_max)
    htmp.Draw("AXIS")
    list_hAmpVsY = []
    legend = TLegend(2*myStyle.GetMargin()+0.02,1-myStyle.GetMargin()-0.02-0.1,1-myStyle.GetMargin()-0.02,1-myStyle.GetMargin()-0.02)
    legend.SetNColumns(3)
    legend.SetTextFont(myStyle.GetFont())
    legend.SetTextSize(myStyle.GetSize())
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.SetFillColor(kWhite) 
    for iCh in range(6):
        list_hAmpVsY.append(inputfile.Get("Amp"+str(iCh)+"VsYLaser"+corr_title+"_X"+str(iX)))
        list_hAmpVsY[iCh].SetMarkerStyle(21)
        list_hAmpVsY[iCh].SetLineColor(color[iCh])
        list_hAmpVsY[iCh].SetLineWidth(3)
        legend.AddEntry(list_hAmpVsY[iCh], "Channel "+str(iCh))
        # list_hAmpVsY[iCh].GetYaxis().SetRangeUser(0.,220.)
        list_hAmpVsY[iCh].Draw("LP")
    legend.Draw()
    canvas.SaveAs("AmpVsY"+corr_title+"_X"+str(iX)+".gif")
    canvas.Clear()
    htmp.Delete()
    # canvas.Delete()
