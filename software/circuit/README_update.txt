The latest version is: powersupply_upd4.pdf


2005-06-25: The circuit diagram contains a minor update
            compared to the original diagram from the article379.
            The diode D2 is now between +out and -out instead of
            +out and gnd.
            The previous connection to gnd had the disadvantage that
            circuit (the atmega8 to be precise) was only protected against
            reverse current up to 1A. That is: you mistreat the powersupply
            and connect a stronger external powersupply to output
            with opposite polarity (incorrect polarity) and a current
            higher than 1A.
            I know that this is not a very common use. It is up to
            you if you want to apply this change.
       
            The new circuit diagram files are called: powersupply_upd.*


2005-07-17: The circuit diagram was updated:
            C6 changed from 22nF to 100nF. There seem to be case
	    where the tolerances of the diodes and transistors 
	    can spread so wide that the circuit becomes unstable
	    (HF oscillations). A 100nF will definitly stabalize the circuit
	    no matter how high the internal parisitic capacities are.
	    100nF is still small enough to get fast response (kHz speed) of
	    the circuit.
	    The higher capacitor will also surpress rest-noise better.

            The new circuit diagram files are called: powersupply_upd2.*

2005-08-05: The circuit diagram was updated: The comment about R39 is wrong
            there is no R39. It should say R35.

            The new circuit diagram files are called: powersupply_upd3.*

2006-01-08: R10 for 30V Version should be 6K8.
            Added R36 (5K6) for protection against current peaks higher
	    than 10A (can not happen under normal conditions but is possible
	    if the power transistor is overheated due to bad cooling and burns. 
	    The resistor will save the live of the microcontroller).

