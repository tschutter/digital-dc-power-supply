Digital DC Power Supply
=======================

This repository contains all documentation and files related to the
digital DC bench power supply kit that I have built.  I purchased the
kit (including a LCD display and three `ATmega8
<http://www.atmel.com/devices/ATMEGA8.aspx>`_ microcontrollers) from
tuxgraphics.org in October 2006 for |euro| 40.

These documents were delivered with the kit:

* `Tuxgraphics Digital DC Power Supply (assembly) <doc/lf-2005_06-0379-assembly.pdf>`_
* `Circuit Diagram (upd4) <doc/powersupply_upd4.pdf>`_
* `Tuxgraphics LCD display 20x2 characters datasheet <doc/tuxgr_20x2.pdf>`_

As of 2015-12-12, there are three separate hardware versions.  I have
hardware version 1.0 which is documented at `linuxfocus.org
<http://linuxfocus.org/English/Archives/>`_.  That site has not been
updated since 2005-12-09, which is why I have collected all relevant
files and documentation here:

* Part 1: The Hardware
  (LinuxFocus `article 379`_)
  (`local copy of 379 <doc/lf-2005_06-0379.pdf>`_)
* Part 2: The Software
  (LinuxFocus `article 384`_)
  (`local copy of 384 <doc/lf-2005_07-0384.pdf>`_)
* Part 3: Command and Control from the PC
  (LinuxFocus `article 389`_)
  (`local copy of 389 <doc/lf-2005_09-0389.pdf>`_)

Build Notes and Decisions
=========================

The 1.0 kit documentation left many choices unclear or up to the
reader.

Power Supply Transformer
------------------------

`Article 379`_ documents two choices when using a transformer and
rectifier as opposed to a laptop power brick, a 22V 2.5A version and a
30V 2.0A version.  The article states that the max output voltage
should be at least 2V lower than the input voltage.

It appears that I chose the 22V option with a lesser max current of
2.0A:

* I purchased a 24V 2.0A transformer
* R10 is 4.7K |ohms|
* Z1 has been replaced by a wire
* C7 was not installed
* R35 was not installed

22V 2.5A
++++++++

* `Article 379`_ specifies a 18V 2.5A transformer (reason: 18 * 1.4 =
  25V), but I believe this is wrong.
* 3000 |micro| F reservoir capacitor (reason: at least 1000 |micro| F
  per ampere)
* R10 should be 4.7K |ohms|
* The circuit diagram says: replace Z1 by a wire for the 24V version
  (maybe typo, should be 22V?)

30V 2.0A
++++++++

* `Article 379`_ specifies a 24V 2.0A transformer (reason: 24 * 1.4 =
  33.6V), but I believe this is wrong.
* 2200 |micro| F reservoir capacitor (reason: at least 1000 |micro| F
  per ampere)
* R10 should be 5.6K |ohms|
* The circuit diagram says: Z1, C7, R35 only for 30V version

Primary Fuse
------------

None of the kit documentation indicated the need for a fuse, but it
sounded like a good idea.

* Choosing the fuse rating is notoriously difficult.
* The fuse should come *before* the switch.  This guards against the
  possibility of the switch failing closed.
* From `Fuses and Fusing @ The Valve Wizard
  <http://www.valvewizard.co.uk/fuses.html>`_:

  * The primary fuse should be rated for around 1.5 to 2 times the
    normal primary operating current. You can work this out by adding
    up the (maximum average) power used by each of the secondary
    windings, then divide by the mains voltage.
  * 1A is typical of most amps up to 60W.
* Normally, you size to protect the transformer.

The transformer rated current = 48VA / 115V = 0.42A.  A slow-blow fuse
of 1A is 2 times the maximum primary operating current and should
allow for transformer inrush current.

Rectifier
---------

`Article 379`_ states:

    A power diodes bridge with 4 diodes which are specified for a low
    voltage drop (e.g BYV29-500) gives a good rectifier.

I sourced a 10PH40 rectifier pack:

* Rated voltage: 400 V
* Output current: 10 A
* Max. forward voltage drop, |V_F| = 1.0 V @ |I_F| = 2.5 A

Reservoir Capacitor
-------------------

According to `article 379
<http://linuxfocus.org/English/June2005/article379.shtml>`_, you need
at least 1000 |micro| F of reservoir capacitor per ampere of input
current.  Given the 2.0 amp transformer, 2000 |micro| F should be
sufficient, but I have used a 3300 |micro| F capacitor.  This is
connected directly to the output of the rectifier, minding the
polarity.

HF Interference
---------------

The `Hardware version 2.0
<http://tuxgraphics.org/electronics/200707/bench-power-supply-unit.shtml>`_
doc suggests soldering a ceramic capacitor in the range of 10 nF to
100 nF directly behind the front output connectors to block HF signals
from interfering with the LCD display electronics.

Connections
-----------

There are 6 sets of external connections on the main PCB labeled CONN1
through CONN6.

* CONN1 - I dunno

  * Pin 1:
  * Pin 2:
  * Pin 3:
  * Pin 4:
  * Pin 5:

* CONN2 - I2C communication to PC (DB9 serial)

  * Pin 1: RTS=7
  * Pin 2: CD=1
  * Pin 3: DTR=4
  * Pin 4: GND=5

* CONN3 - LCD Display

  * 10 pins

* CONN4 - DC Power Out

  * Pin 1: DC Power Out -
  * Pin 2: DC Power Out +

* CONN5 - Front Panel Switches

  * Pin 1: S1 Voltage +
  * Pin 2: S2 Voltage -
  * Pin 3: S3 Current +, S5 Store
  * Pin 4: S4 Current -
  * Pin 5: S[1-4] Current and voltage common
  * Pin 6: S5 Store

* CONN6 - DC Power In

  * Pin 1: DC Power In +
  * Pin 2: DC Power In Ground
  * Pin 3: DC Power In -

Parts List
==========

* `Triad F-192X Power Transformer <http://www.mouser.com/ProductDetail/Triad-Magnetics/F192X/>`_

  * Maximum Power: 48VA
  * Primary: 115V 50/60Hz
  * Secondary: 24.0VCT @ 2.0A

* 10PH40 Rectifier Pack

  * Single Phase, Full Wave Bridge
  * Rated Voltage: 400V
  * Output Current: 10A
  * Max. Forward Voltage Drop, V_F = 1.0V @ I_F = 2.5A

* 3300 |micro| F capacitor (reservoir for rectifier bridge)

* `Eagle Plastic Devices Enclosure 40UB103 <http://www.mouser.com/ProductDetail/Eagle-Plastic-Devices/40UB103/>`_

  * Width: 134mm
  * Depth: 150mm
  * Height: 77mm

Software
========

`Version 0.4.9 of the software <digitaldcpower-0.4.9/>`_ was
downloaded from the `LinuxFocus article 389 download page
<http://linuxfocus.org/common/src2/article389/>`_.

Related Stuff
=============

* `Hardware version 2.0 <http://tuxgraphics.org/electronics/200707/bench-power-supply-unit.shtml>`_
* `Hardware version 3.0 <http://www.tuxgraphics.org/electronics/201005/bench-power-supply-v3.shtml>`_
* `Design Guide for Rectifier Use <http://www.hammondmfg.com/pdf/5c007.pdf>`_

.. reStructuredText definitions
.. |euro| unicode:: 0x20AC .. copyright sign
   :rtrim:
.. |ohms| unicode:: 0x03A9 .. Greek uppercase omega
   :ltrim:
.. |micro| unicode:: 0x00B5 .. Greek lowercase mu
   :ltrim:
   :rtrim:
.. |I_F| replace:: I\ :sub:`F`
.. |V_F| replace:: V\ :sub:`F`
.. _article 379: http://linuxfocus.org/English/June2005/article379.shtml
.. _article 384: http://linuxfocus.org/English/July2005/article384.shtml
.. _article 389: http://linuxfocus.org/English/September2005/article389.shtml
