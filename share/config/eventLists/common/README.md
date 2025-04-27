Eventlists
=========================

Eventlists are tables with events, one event per row, listing certain
properties of the events in each column. They can be booked similar to histograms.

Usage
--------------------

The syntax for these files is similar to the one used for histogramming. An event list called "evtlist" is defined with four columns, containing run and event numbers, invariant dijet mass (in GeV) and pseudorapidity gap between the jets.

    evtlist: Run << RunNumber, Event << EventNumber, \ensuremath{m_{jj}} << Mjj/1000. , \ensuremath{\Delta y_{jj}} << DYjj; 

The event list "evtlist" is appended to the cut named `Cut_2jetincl`. 

    @Cut_2jetincl: evtlist; 