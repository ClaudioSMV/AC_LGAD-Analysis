from ROOT import TFile,TTree,TCanvas,TH1F,TH2F,TGaxis,TLatex,TMath,TEfficiency,TGraphAsymmErrors,TLegend, TColor,gPad,gROOT,gStyle, kWhite
import os
import optparse
import myStyle

gROOT.SetBatch( True )
# gStyle.SetOptFit(1011)
gStyle.SetOptStat(0)

## Defining Style
myStyle.ForceStyle()
# gStyle.SetTitleYOffset(1.1)

# def ReverseYAxis(h):
#    # Remove the current axis
#    h.GetYaxis().SetLabelOffset(999)
#    h.GetYaxis().SetTickLength(0)
 
#    # Redraw the new axis
#    gPad.Update()
#    newaxis = TGaxis(gPad.GetUxmin(), gPad.GetUymax(), gPad.GetUxmin()-0.001, gPad.GetUymin(), h.GetYaxis().GetXmin(), h.GetYaxis().GetXmax(), 510, "+")
#    newaxis.SetLabelOffset(-0.03)
#    newaxis.Draw()

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
color_rgb = [[0,255,0], [0,255,255], [0,0,255], [204,0,255], [255,0,0], [255,255,0]]
color_chg = [[0,-20,0], [0,-20,0], [0,0,-20], [0,20,0], [-20,0,0], [0,-20,0]]
# color_RGB = [[51,34,136],[51,187,238],[17,119,51],[153,153,51],[204,102,119],[136,34,85]]
# # [indigo, cyan, green, olive, rose, wine]
# color_list = []

# for i in range(0,len(color_RGB)):
#     c = TColor.GetColor(color_RGB[i][0],color_RGB[i][1],color_RGB[i][2])
#     color_list.append(c)

canvas = TCanvas("cv", "cv", 1000, 800)

htmp = inputfile.Get("hAmpVsXY"+corr_title+"_Ch0").Clone("htmp") # hAmpVsXY_Corr_Ch%i
ymin = htmp.GetYaxis().GetXmin()
ymax = htmp.GetYaxis().GetXmax()
nxbins = htmp.GetXaxis().GetNbins()
xmin = htmp.GetXaxis().GetXmin()
xmax = htmp.GetXaxis().GetXmax()
nybins = htmp.GetYaxis().GetNbins()
htmp.Delete()

# if ("MultiSnsr" in file):
#     amp_max = 180.
# elif ("EIC" in file):
#     amp_max = 250.

gStyle.SetOptTitle()
xy_range = myStyle.GetSensorRange(file)

## Time Vs Y, per X=constant bin
for iX in range(nxbins):
    # htmp.SetTitle("Amplitude"+corr_title+" Vs Y_laser, X["+str(iX)+"];y_laser [mm];amp"+corr_title+" [mV]")
    htmp = TH1F("htmp", "Amp"+corr_title+" Vs Y_laser, X="+str(xy_range[0][iX])+" [mm];y_laser [mm];Amp"+corr_title+" [mV]", 1, ymin, ymax)
    htmp.GetYaxis().SetRangeUser(0.0, 300.0) # Check if these values can be retrieved with ~ htmp.GetZaxis().GetXmax()
    htmp.Draw("AXIS")
    # htmp.Draw("X+")
    # ReverseYAxis(htmp)
    # list_hTimeVsY = []
    legend = TLegend(2*myStyle.GetMargin()+0.02,1-myStyle.GetMargin()-0.02-0.1,1-myStyle.GetMargin()-0.02,1-myStyle.GetMargin()-0.02)
    legend.SetNColumns(3)
    legend.SetTextFont(myStyle.GetFont())
    legend.SetTextSize(myStyle.GetSize())
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.SetFillColor(kWhite) 
    for iCh in range(6):
        hAmpVsY = inputfile.Get("hAmpVsXY"+corr_title+"_Ch"+str(iCh)).ProjectionY("hAmpVsY"+corr_title+"_Ch"+str(iCh),iX+1,iX+1)
        hAmpVsY.SetMarkerStyle(21)
        hAmpVsY.SetLineColor(color[iCh])
        hAmpVsY.SetLineWidth(3)
        legend.AddEntry(hAmpVsY, "Channel "+str(iCh))
        # list_hAmpVsY[iCh].GetYaxis().SetRangeUser(0.,220.)
        hAmpVsY.Draw("SAME HIST LP")
        # hAmpVsY.Delete() # ???
    legend.Draw()
    canvas.SaveAs("AmpVsY"+corr_title+"_X"+str(iX)+".gif")
    canvas.Clear()
    htmp.Delete()
    # canvas.Delete()

## Time Vs X, per Y=constant bin
for iY in range(nybins):
    htmp = TH1F("htmp", "Amp"+corr_title+" Vs X_laser, Y="+str(xy_range[1][iY])+" [mm];x_laser [mm];Amp"+corr_title+" [mV]", 1, xmin, xmax)
    htmp.GetYaxis().SetRangeUser(0.0, 300.0) # Check if these values can be retrieved with ~ htmp.GetZaxis().GetXmax()
    htmp.Draw("AXIS")
    # htmp.Draw("X+")
    # ReverseYAxis(htmp)
    # list_hTimeVsY = []
    legend = TLegend(2*myStyle.GetMargin()+0.02,1-myStyle.GetMargin()-0.02-0.1,1-myStyle.GetMargin()-0.02,1-myStyle.GetMargin()-0.02)
    legend.SetNColumns(3)
    legend.SetTextFont(myStyle.GetFont())
    legend.SetTextSize(myStyle.GetSize())
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.SetFillColor(kWhite) 
    for iCh in range(6):
        hAmpVsX = inputfile.Get("hAmpVsXY"+corr_title+"_Ch"+str(iCh)).ProjectionX("hAmpVsX"+corr_title+"_Ch"+str(iCh),iY+1,iY+1)
        hAmpVsX.SetMarkerStyle(21)
        hAmpVsX.SetLineColor(color[iCh])
        hAmpVsX.SetLineWidth(3)
        legend.AddEntry(hAmpVsX, "Channel "+str(iCh))
        # list_hAmpVsX[iCh].GetYaxis().SetRangeUser(0.,220.)
        hAmpVsX.Draw("SAME HIST LP")
        # hAmpVsX.Delete() # ???
    legend.Draw()
    canvas.SaveAs("AmpVsX"+corr_title+"_Y"+str(iY)+".gif")
    canvas.Clear()
    htmp.Delete()
    # canvas.Delete()

## Time Vs X, per channel (all Y values in a single plot for that channel)
for iCh in range(6):
    htmp = TH1F("htmp", "Amp"+corr_title+" Vs X_laser, Channel "+str(iCh)+";x_laser [mm];Amp"+corr_title+" [mV]", 1, xmin, xmax)
    htmp.GetYaxis().SetRangeUser(0.0, 300.0) # Check if these values can be retrieved with ~ htmp.GetZaxis().GetXmax()
    htmp.Draw("AXIS")
    legend = TLegend(2*myStyle.GetMargin()+0.02,1-myStyle.GetMargin()-0.02-0.2,1-myStyle.GetMargin()-0.02,1-myStyle.GetMargin()-0.02)
    legend.SetNColumns(4)
    legend.SetTextFont(myStyle.GetFont())
    legend.SetTextSize(myStyle.GetSize())
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.SetFillColor(kWhite)

    strip_position = myStyle.ChannelPos(file, iCh)
    for iY in range(1, nybins-1):
        hAmpVsX = inputfile.Get("hAmpVsXY"+corr_title+"_Ch"+str(iCh)).ProjectionX("hAmpVsX"+corr_title+"_Ch"+str(iCh)+str(iY),iY+1,iY+1)
        if iY in strip_position: hAmpVsX.SetMarkerStyle(26)
        elif ((iY-1) in strip_position) or ((iY+1) in strip_position): hAmpVsX.SetMarkerStyle(25)
        else: hAmpVsX.SetMarkerStyle(21)
        col = TColor.GetColor(color_rgb[iCh][0] + (iY-1)*color_chg[iCh][0], color_rgb[iCh][1] + (iY-1)*color_chg[iCh][1], color_rgb[iCh][2] + (iY-1)*color_chg[iCh][2])
        hAmpVsX.SetLineColor(col)
        hAmpVsX.SetLineWidth(3)
        legend.AddEntry(hAmpVsX, "Y = "+str(xy_range[1][iY]))
        # list_hAmpVsX[iCh].GetYaxis().SetRangeUser(0.,220.)
        hAmpVsX.Draw("SAME HIST LP")
    legend.Draw()
    canvas.SaveAs("AmpVsX"+corr_title+"_Ch"+str(iCh)+".gif")
    canvas.Clear()
    htmp.Delete()
