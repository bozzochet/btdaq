#!/usr/bin/env ruby

$LOAD_PATH.unshift(ENV['AMSDir'] + "/ruby/linux")

require 'AMS'
include AMS

app = TRint.new("My application", 0, [])

hfile = TFile.new("amstest.root","RECREATE","AMSRuby test")
hist = TH1F.new("hist","Z vertex position (cm)",25,-50.0,150.0)

ams = AMSChain.new
ams.Add("/f2users/choutko/g3v1g3.root")
#ams.Add("http://pcamsf0.cern.ch/f2dah1/MC/AMS02/2004A/protons/el.pl1.10200/738197524.0000001.root")
ndata = ams.GetEntries.to_i

ndata.times do
  ev = ams.GetEvent
#  if ev.nParticle==1
#      puts ev.Particle(0).Info
#  end
  if ev.nVertex == 1
      vtx = ev.Vertex(0)
      zpos = vtx.Vertex[2]
      hist.Fill(zpos)
  end
end

hist.Fit("gaus")

AMS.gStyle.SetOptStat(0)
AMS.gStyle.SetOptFit(1111)
hist.Draw

hfile.Write

app.Run
