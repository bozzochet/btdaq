#!/usr/bin/env ruby
# Try to run this interactively with "irb -r ams_example2.rb"

$LOAD_PATH.unshift(ENV['AMSDir'] + "/ruby/linux")

require 'AMS'
include AMS

app = TApplication.new("My application", 0, [])

hfile = TFile.new("amstest.root","RECREATE","AMSRuby test")
hist = TH1F.new("hist","Z vertex position (cm)",25,-50.0,150.0)

ams = AMSChain.new
ams.Add("/f2users/choutko/g3v1g3.root")
ndata = ams.GetEntries.to_i

ndata.times do
  ev = ams.GetEvent
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

pid = Process.fork{app.Run}

END { Process.kill("KILL",pid) ; app.Terminate }
