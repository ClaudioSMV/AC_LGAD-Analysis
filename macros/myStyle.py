import ROOT

## Define global variables
marg=0.05
font=43 # Helvetica
# tsize=32
tsize=38 #35

def ForceStyle():
    ## Defining Style
    ROOT.gStyle.SetPadTopMargin(marg)    #0.05
    ROOT.gStyle.SetPadRightMargin(marg)  #0.05
    ROOT.gStyle.SetPadBottomMargin(2*marg)  #0.16
    ROOT.gStyle.SetPadLeftMargin(2*marg)   #0.16

    ROOT.gStyle.SetPadTickX(1)
    ROOT.gStyle.SetPadTickY(1)

    ROOT.gStyle.SetTextFont(font)
    ROOT.gStyle.SetLabelFont(font,"x")
    ROOT.gStyle.SetTitleFont(font,"x")
    ROOT.gStyle.SetLabelFont(font,"y")
    ROOT.gStyle.SetTitleFont(font,"y")
    ROOT.gStyle.SetLabelFont(font,"z")
    ROOT.gStyle.SetTitleFont(font,"z")

    ROOT.gStyle.SetTextSize(tsize)
    ROOT.gStyle.SetLabelSize(tsize-4,"x")
    ROOT.gStyle.SetTitleSize(tsize,"x")
    ROOT.gStyle.SetLabelSize(tsize-4,"y")
    ROOT.gStyle.SetTitleSize(tsize,"y")
    ROOT.gStyle.SetLabelSize(tsize-4,"z")
    ROOT.gStyle.SetTitleSize(tsize,"z")

    ROOT.gStyle.SetLegendFont(font)
    ROOT.gStyle.SetLegendTextSize(tsize)
    ROOT.gStyle.SetLegendBorderSize(0)
    ROOT.gStyle.SetLegendFillColor(0)

    ROOT.gStyle.SetTitleXOffset(1.0)
    ROOT.gStyle.SetTitleYOffset(1.0)
    ROOT.gStyle.SetOptTitle(0)
    # ROOT.gStyle.SetOptStat(0)

    ROOT.gStyle.SetHistLineWidth(4)

    ROOT.gStyle.SetGridColor(921)
    ROOT.gStyle.SetGridStyle()

    ROOT.gROOT.ForceStyle()

def BeamInfo():
    text = ROOT.TLatex()
    text.SetTextSize(tsize-4)
    text.DrawLatexNDC(2*marg+0.005,1-marg+0.01,"#bf{FNAL 120 GeV proton beam}")

def SensorInfo(sensor="Name", bias_voltage="X", write_bv=True):
    text = ROOT.TLatex()
    text.SetTextSize(tsize-4)
    text.SetTextAlign(31)
    if write_bv: text.DrawLatexNDC(1-marg-0.005,1-marg+0.01,"#bf{"+str(sensor) + ", "+str(bias_voltage)+"V}")
    else: text.DrawLatexNDC(1-marg-0.005,1-marg+0.01,"#bf{"+str(sensor)+"}")

def GetMargin():
    return marg

def GetFont():
    return font

def GetSize():
    return tsize

def GetPadCenter():
    return (1 + marg)/2

def GetSensorRange(file):
    ## Sensor: LaserMultiSnsr
    if "LaserMultiSnsr" in file:
        xy_range = [[36.5, 37.0, 37.5, 38.0, 38.5, 39.0, 39.5], [14.05, 14.30, 14.55, 14.80, 15.05, 15.30, 15.55, 15.80, 16.05, 16.30, 16.55, 16.80, 17.05]]

    ## Sensor: EIC1p0CM
    elif "EIC1p0CM" in file:
        xy_range = [[31.6, 32.1, 32.6, 33.1, 33.6, 34.1, 34.6, 35.1, 35.6, 36.1, 36.6, 37.1, 37.6, 38.1, 38.6, 39.1, 39.6, 40.1, 40.6, 41.1, 41.6, 42.1], [12.60, 12.85, 13.10, 13.35, 13.60, 13.85, 14.10, 14.35, 14.60, 14.85, 15.10, 15.35, 15.60, 15.85, 16.10, 16.35, 16.60, 16.85, 17.10, 17.35, 17.60]]

    ## Test with run_scope9183.root and run_scope9184.root
    else:
        xy_range = [[38.0], [14.30, 14.55]]

    return xy_range

def ChannelPos(file, channel):
    ## Sensor: LaserMultiSnsr
    if "LaserMultiSnsr" in file:
        y_pos = [[1,2], [2,3], [4,5], [5,6], [6], [7]]

    ## Sensor: EIC1p0CM
    elif "EIC1p0CM" in file:
        y_pos = [[2,3], [4,5], [6,7], [8,9], [16,17], [18,19]]

    ## Test with run_scope9183.root and run_scope9184.root
    else:
        y_pos = [[1,2], [2,3], [4,5], [5,6], [6], [7]]

    return y_pos[channel]