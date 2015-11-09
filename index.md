---
title: VME DAQBox
layout: basic
---

## VME DAQBox

This software package describes software designed to interface with a VME
DAQBox to readout
[CDT Cascade detectors](http://n-cdt.com/products/cascade-detector-systems/).

The C++ libraries interface with `libHardwareLib` (from CDT), allowing
configuration and readout with JSON.  This is then exported to a web interface
using [autobahn](http://autobahn.ws/) and a corresponding web server is run
using [crossbar.io](http://crossbar.io/).


### Build/Installation

The `Makefile` (at top level) needs to be edited to give the path to
`libHardwareLib` and the associated header files.  Then simply:

{% highlight sh %}
./configure
make
{% endhighlight %}

should be enough to build the files.  After this, one can also start a crossbar
server to begin the web and web socket servers:

{% highlight sh %}
crossbar start
make
{% endhighlight %}

This is of course assuming crossbar is installed.  At this point, one should be
able to connect to the server: `http://ip_of_sbc:8080`.  Also, one can look at
[this jsfiddle](https://jsfiddle.net/mgmarino/akrn5uqb/12/embedded/result/) to
see an example how to build a web interface with display, etc.  Note that the
jsfiddle also uses as default server 10.155.59.160, which should work if the
SBC in cluster is running.

A full screen version of the example is
[available here](http://jsfiddle.net/mgmarino/akrn5uqb/embedded/result/).

### Overview

The code here works to essentially convert HardwareLib structs to JSON objects to
enable passing them across a web interface (or from a python instance).  The
following structs are convertible/converted to/from JSON:

* CFConfig
* CFSimulation
* CFStatistics
* CFDiagnostics
* HSetup
* ChannelList
* MConfig

### SBC Code

Code to run the card is available on the CASCADE detectors SBC image
[here]({{ site.url }}/System-Overview/subsystems/SBCs.html#backup-images).
This script to start the program as a [runit
daemon](http://smarden.org/runit/faq.html) looks simply like:

{% highlight sh %}
#!/bin/sh

exec sudo -u daq \
   /usr/bin/crossbar start \
   --cbdir /home/daq/VMEDAQBox/.crossbar \
   --logdir /home/daq/VMEDAQBox/.crossbar/log
{% endhighlight %}


