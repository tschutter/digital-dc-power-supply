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

* `Part 1: The Hardware <doc/lf-2005_06-0379.pdf>`_
  (`LinuxFocus article 379
  <http://linuxfocus.org/English/June2005/article379.shtml>`_)
* `Part 2: The Software <doc/lf-2005_07-0384.pdf>`_
  (`LinuxFocus article 384
  <http://linuxfocus.org/English/July2005/article384.shtml>`_)
* `Part 3: Command and Control from the PC <doc/lf-2005_09-0389.pdf>`_
  (`LinuxFocus article 389
  <http://linuxfocus.org/English/September2005/article389.shtml>`_)

`Version 0.4.9 of the software <digitaldcpower-0.4.9/>`_ was
downloaded from the `LinuxFocus article 389 download page
<http://linuxfocus.org/common/src2/article389/>`_.

Modifications
-------------

* The `Hardware version 2.0
  <http://tuxgraphics.org/electronics/200707/bench-power-supply-unit.shtml>`_
  doc suggests soldering a ceramic capacitor in the range of 10nF to
  100nF directly behind the front output connectors to block HF
  signals from interfering with the LCD display electronics.

Parts List
----------

* `Triad F-192X Power Transformer <http://www.mouser.com/ProductDetail/Triad-Magnetics/F192X/>`_

  * Maximum Power: 48 VA
  * Primary: 115 V 50/60 Hz
  * Secondary: 24.0 VCT @ 2.0 Amps

* `Eagle Plastic Devices Enclosure 40UB103 <http://www.mouser.com/ProductDetail/Eagle-Plastic-Devices/40UB103/>`_

  * 134 mm wide
  * 150 mm deep
  * 77 mm high

Related Stuff
-------------

* `Hardware version 2.0 <http://tuxgraphics.org/electronics/200707/bench-power-supply-unit.shtml>`_
* `Hardware version 3.0 <http://www.tuxgraphics.org/electronics/201005/bench-power-supply-v3.shtml>`_

.. reStructuredText definitions
.. |euro| unicode:: 0x20AC .. copyright sign
   :rtrim:
