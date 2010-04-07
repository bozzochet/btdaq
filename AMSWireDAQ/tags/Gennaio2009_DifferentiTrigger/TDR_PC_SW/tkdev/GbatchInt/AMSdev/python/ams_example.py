#!/usr/bin/env python2.4

import sys, os
sys.path.insert(0,os.environ['AMSDir'] + '/python/linux')

from AMS import TRint, TFile, TH1F, AMSChain, cvar

app = TRint("My application", 0, [])

hfile = TFile("amstest.root","RECREATE","AMSPython test")

hist = TH1F("hist","Z vertex position (cm)",25,-50.0,150.0)

ams = AMSChain()
#ams.Add("/f2users/choutko/g3v1g3.root")
ams.Add("http://pcamsf0.cern.ch/f2dah1/MC/AMS02/2004A/protons/el.pl1.10200/738197524.0000001.root")
ndata = ams.GetEntries()

for i in range(ndata):
  ev = ams.GetEvent()
#  if ev.nParticle()==1:
#      print ev.Particle(0).Info()
  if ev.nVertex() == 1:
      vtx = ev.Vertex(0)
      zpos = vtx.Vertex[2]
      hist.Fill(zpos) 

hist.Fit("gaus")

cvar.gStyle.SetOptStat(0)
cvar.gStyle.SetOptFit(1111)
hist.Draw()

hfile.Write()

print "We have read and analyzed %d entries" % ndata

app.Run()
