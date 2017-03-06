#!/bin/sh
qucsconv -if qucsdata -of touchstone -i short.dat -o short_750mm.s1p
qucsconv -if qucsdata -of touchstone -i load.dat -o load_750mm.s1p
qucsconv -if qucsdata -of touchstone -i open.dat -o open_750mm.s1p
qucsconv -if qucsdata -of touchstone -i through.dat -o through_750mm.s2p
qucsconv -if qucsdata -of touchstone -i isolation.dat -o isolation_750mm.s2p
qucsconv -if qucsdata -of touchstone -i dut_ideal.dat -o amp_ideal.s2p
qucsconv -if qucsdata -of touchstone -i dut_750mm.dat -o amp_750mm.s2p

