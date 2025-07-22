cmake_minimum_required(VERSION 3.16)

# project ( signal_devel )
project ( apeal-signal )



include(../../scripts/tests.cmake)


#addTest(signal_common)
addTest(signal_init)
addTest(signal_move)
addTest(signal_noncopy)
addTest(signal_ref)

