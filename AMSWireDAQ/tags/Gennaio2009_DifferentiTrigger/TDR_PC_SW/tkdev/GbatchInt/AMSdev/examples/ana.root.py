#!/usr/bin/env python2.2

# The following two lines are not necessary if the AMS module is
# already installed as a general Python module 
import sys, os
sys.path.insert(0,os.environ['AMSDir'] + '/python/linux')

from AMS import AMSChain, AMSEventList, TRint, TFile, TH1F

app = TRint("", 0, [])

ams = AMSChain()
ams.Add("/f2users/choutko/g3v1g3.root")
#ams.Add("http://pcamsf0.cern.ch/f2dah1/MC/AMS02/2004A/protons/el.pl1.10200/738197524.0000001.root");
#ams.Add("rfio:/castor/cern.ch/ams/MC/AMS02/2004A/protons/el.pl1.10200/738197524.0000001.root");

hfile = TFile("amstest.root", "RECREATE")

list = AMSEventList()

hrig = TH1F("hrig", "Momentum (GeV)", 50, -10., 10.)

ndata = ams.GetEntries()

for entry in range(ndata):
  ev = ams.GetEvent()
  if not ev: break
  for i in range(ev.nParticle()):
      part = ev.Particle(i)
      hrig.Fill(part.Momentum) 
      if ev.nVertex() > 0:
            # Add to list of selected events
            list.Add(ev)
            # Write it into ouput ROOT file
            ev.Fill()

hrig.Draw()
hfile.Write()
list.Write("selected.list")

print "\n>>> We have processed %d events" % ndata
print "\n>>> Histograms saved in '%s'" % hfile.GetName()

app.Run()
