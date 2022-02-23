from ROOT import TFile,TTree,TCanvas,TH1F,TH2F,TGaxis,TLatex,TMath,TEfficiency,TGraphAsymmErrors,TLegend,gPad,gROOT,gStyle, kWhite
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
# parser.add_option('-n','--nocorrection', action="store_true", dest='nocorrection', default = False, help="Apply [default] or not factor correction to amp")
# parser.add_option('-b','--biasvolt', dest='biasvolt', default = 220, help="Bias Voltage value in [V]")
options, args = parser.parse_args()

file = options.file
# nocorrection = options.nocorrection
# bias = options.biasvolt

inputfile = TFile("../"+file,"READ")

color = [416+2, 432+2, 600, 880, 632, 400+2]
color_channel = [416+4, 432+4, 600+2, 616+4, 632+4, 400+4]

canvas = TCanvas("cv", "cv", 1000, 800)

htmp = inputfile.Get("hTimeVsXY_Ch0").Clone("htmp") # hTimeVsXY_Ch%i
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

x_minimum_pCh = [9999, 9999, 9999, 9999, 9999, 9999]
y_minimum_pCh = [9999, 9999, 9999, 9999, 9999, 9999]

## Time Vs Y, per X=constant bin
for iX in range(nxbins):
    # htmp.SetTitle("Amplitude"+corr_title+" Vs Y_laser, X["+str(iX)+"];y_laser [mm];amp"+corr_title+" [mV]")
    htmp = TH1F("htmp", "#DeltaTime Vs Y_laser, X="+str(xy_range[0][iX])+" [mm];y_laser [mm];#Deltat[ns]", 1, ymin, ymax)
    htmp.GetYaxis().SetRangeUser(54.5, 56.5) # Check if these values can be retrieved with ~ htmp.GetZaxis().GetXmax()
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
        hTimeVsY = inputfile.Get("hTimeVsXY_Ch"+str(iCh)).ProjectionY("hTimeVsY_Ch"+str(iCh),iX+1,iX+1)
        hTimeVsY.SetMarkerStyle(21)
        hTimeVsY.SetLineColor(color[iCh])
        hTimeVsY.SetLineWidth(3)
        legend.AddEntry(hTimeVsY, "Channel "+str(iCh))
        # list_hTimeVsY[iCh].GetYaxis().SetRangeUser(0.,220.)
        hTimeVsY.Draw("SAME HIST LP")
        tmp_min = hTimeVsY.GetMinimum(0.1)
        if tmp_min<y_minimum_pCh[iCh]: y_minimum_pCh[iCh] = tmp_min
        # hTimeVsY.Delete() # ???
    legend.Draw()
    canvas.SaveAs("TimeVsY_X"+str(iX)+".gif")
    canvas.Clear()
    htmp.Delete()
    # canvas.Delete()

## Time Vs X, per Y=constant bin
for iY in range(nybins):
    htmp = TH1F("htmp", "#DeltaTime Vs X_laser, Y="+str(xy_range[1][iY])+" [mm];x_laser [mm];#Deltat[ns]", 1, xmin, xmax)
    htmp.GetYaxis().SetRangeUser(54.5, 56.5) # Check if these values can be retrieved with ~ htmp.GetZaxis().GetXmax()
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
        hTimeVsX = inputfile.Get("hTimeVsXY_Ch"+str(iCh)).ProjectionX("hTimeVsX_Ch"+str(iCh),iY+1,iY+1)
        hTimeVsX.SetMarkerStyle(21)
        hTimeVsX.SetLineColor(color[iCh])
        hTimeVsX.SetLineWidth(3)
        legend.AddEntry(hTimeVsX, "Channel "+str(iCh))
        # list_hTimeVsX[iCh].GetYaxis().SetRangeUser(0.,220.)
        tmp_min = hTimeVsX.GetMinimum(0.1)
        if tmp_min<x_minimum_pCh[iCh]: x_minimum_pCh[iCh] = tmp_min
        hTimeVsX.Draw("SAME HIST LP")
        # hTimeVsX.Delete() # ???
    legend.Draw()
    canvas.SaveAs("TimeVsX_Y"+str(iY)+".gif")
    canvas.Clear()
    htmp.Delete()
    # canvas.Delete()

## Time Vs X, per channel (all Y values in a single plot for that channel)
for iCh in range(6):
    htmp = TH1F("htmp", "#DeltaTime Vs X_laser, Channel "+str(iCh)+";x_laser [mm];#Deltat[ns]", 1, xmin, xmax)
    htmp.GetYaxis().SetRangeUser(54.5, 56.5) # Check if these values can be retrieved with ~ htmp.GetZaxis().GetXmax()
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
        hTimeVsX = inputfile.Get("hTimeVsXY_Ch"+str(iCh)).ProjectionX("hTimeVsX_Ch"+str(iCh)+str(iY),iY+1,iY+1)
        if iY in strip_position: hTimeVsX.SetMarkerStyle(25)
        else: hTimeVsX.SetMarkerStyle(21)
        hTimeVsX.SetLineColor(color_channel[iCh]-iY-1)
        hTimeVsX.SetLineWidth(3)
        legend.AddEntry(hTimeVsX, "y = "+str(xy_range[1][iY]))
        # list_hTimeVsX[iCh].GetYaxis().SetRangeUser(0.,220.)
        hTimeVsX.Draw("SAME HIST LP")
    legend.Draw()
    canvas.SaveAs("TimeVsX_Ch"+str(iCh)+".gif")
    canvas.Clear()
    htmp.Delete()

print("xMin: %.2f, %.2f, %.2f, %.2f, %.2f, %.2f" % (x_minimum_pCh[0], x_minimum_pCh[1], x_minimum_pCh[2], x_minimum_pCh[3], x_minimum_pCh[4], x_minimum_pCh[5]))
print("yMin: %.2f, %.2f, %.2f, %.2f, %.2f, %.2f" % (y_minimum_pCh[0], y_minimum_pCh[1], y_minimum_pCh[2], y_minimum_pCh[3], y_minimum_pCh[4], y_minimum_pCh[5]))

## Time Vs Y, per X=constant bin with Time set to zero per channel
for iX in range(nxbins):
    htmp = TH1F("htmp", "#DeltaTime_SetZero Vs Y_laser, X="+str(xy_range[0][iX])+" [mm];y_laser [mm];#Deltat[ns]", 1, ymin, ymax)
    htmp.GetYaxis().SetRangeUser(-0.01, 1.01)
    htmp.Draw("AXIS")
    legend = TLegend(2*myStyle.GetMargin()+0.02,1-myStyle.GetMargin()-0.02-0.1,1-myStyle.GetMargin()-0.02,1-myStyle.GetMargin()-0.02)
    legend.SetNColumns(3)
    legend.SetTextFont(myStyle.GetFont())
    legend.SetTextSize(myStyle.GetSize())
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.SetFillColor(kWhite) 
    for iCh in range(6):
        strip_position = myStyle.ChannelPos(file, iCh)
        hTimeVsY = inputfile.Get("hTimeVsXY_Ch"+str(iCh)).ProjectionY("hTimeVsY_Ch"+str(iCh),iX+1,iX+1)
        for iY in range(nybins):
            time_value = hTimeVsY.GetBinContent(iY+1)
            hTimeVsY.SetBinContent(iY+1, time_value - y_minimum_pCh[iCh])
        hTimeVsY.SetMarkerStyle(21)
        hTimeVsY.SetLineColor(color[iCh])
        hTimeVsY.SetLineWidth(3)
        legend.AddEntry(hTimeVsY, "Channel "+str(iCh))
        hTimeVsY.Draw("SAME HIST LP")
    legend.Draw()
    canvas.SaveAs("TimeVsY_Zero_X"+str(iX)+".gif")
    canvas.Clear()
    htmp.Delete()
    # canvas.Delete()

## Time Vs X, per Y=constant bin with Time set to zero per channel
for iY in range(nybins):
    htmp = TH1F("htmp", "#DeltaTime_SetZero Vs X_laser, Y="+str(xy_range[1][iY])+" [mm];x_laser [mm];#Deltat[ns]", 1, xmin, xmax)
    htmp.GetYaxis().SetRangeUser(-0.01, 1.01) # Check if these values can be retrieved with ~ htmp.GetZaxis().GetXmax()
    htmp.Draw("AXIS")
    legend = TLegend(2*myStyle.GetMargin()+0.02,1-myStyle.GetMargin()-0.02-0.1,1-myStyle.GetMargin()-0.02,1-myStyle.GetMargin()-0.02)
    legend.SetNColumns(3)
    legend.SetTextFont(myStyle.GetFont())
    legend.SetTextSize(myStyle.GetSize())
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.SetFillColor(kWhite) 
    for iCh in range(6):
        strip_position = myStyle.ChannelPos(file, iCh)
        hTimeVsX = inputfile.Get("hTimeVsXY_Ch"+str(iCh)).ProjectionX("hTimeVsX_Ch"+str(iCh),iY+1,iY+1)
        for iX in range(nxbins):
            time_value = hTimeVsX.GetBinContent(iX+1)
            hTimeVsX.SetBinContent(iX+1, time_value - x_minimum_pCh[iCh])
        if iY in strip_position: hTimeVsX.SetMarkerStyle(26)
        elif ((iY-1) in strip_position) or ((iY+1) in strip_position): hTimeVsX.SetMarkerStyle(25)
        else: hTimeVsX.SetMarkerStyle(21)
        hTimeVsX.SetLineColor(color[iCh])
        hTimeVsX.SetLineWidth(3)
        legend.AddEntry(hTimeVsX, "Channel "+str(iCh))
        hTimeVsX.Draw("SAME HIST LP")
    legend.Draw()
    canvas.SaveAs("TimeVsX_Zero_Y"+str(iY)+".gif")
    canvas.Clear()
    htmp.Delete()
    # canvas.Delete()

## Time Vs X, per channel (all Y values in a single plot for that channel), with Time set to zero per channel
for iCh in range(6):
    htmp = TH1F("htmp", "#DeltaTime_SetZero Vs X_laser, Channel "+str(iCh)+";x_laser [mm];#Deltat[ns]", 1, xmin, xmax)
    htmp.GetYaxis().SetRangeUser(-0.01, 1.21)
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
        hTimeVsX = inputfile.Get("hTimeVsXY_Ch"+str(iCh)).ProjectionX("hTimeVsX_Ch"+str(iCh)+str(iY),iY+1,iY+1)
        for iX in range(nxbins):
            time_value = hTimeVsX.GetBinContent(iX+1)
            hTimeVsX.SetBinContent(iX+1, time_value - x_minimum_pCh[iCh])
        if iY in strip_position: hTimeVsX.SetMarkerStyle(26)
        elif ((iY-1) in strip_position) or ((iY+1) in strip_position): hTimeVsX.SetMarkerStyle(25)
        else: hTimeVsX.SetMarkerStyle(21)
        hTimeVsX.SetLineColor(color_channel[iCh]-iY-1)
        hTimeVsX.SetLineWidth(3)
        legend.AddEntry(hTimeVsX, "y = "+str(xy_range[1][iY]))
        # list_hTimeVsX[iCh].GetYaxis().SetRangeUser(0.,220.)
        hTimeVsX.Draw("SAME HIST LP")
    legend.Draw()
    canvas.SaveAs("TimeVsX_Zero_Ch"+str(iCh)+".gif")
    canvas.Clear()
    htmp.Delete()
