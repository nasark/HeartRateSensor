# HeartRateSensor
Embedded system capable of detecting user's heart rate and displaying BPM, built using C.

## Background
The user's heartbeat is serially communicated using a transducer. The voltage received as a result of the heartbeat is then coverted into a digital value. If the digital value exceeds a determined threshold, a heartbeat is considered to be detected and this is then used in the algorithm to calculate the BPM (beats per minute). The BPM is displayed on LEDs in Binary-Coded Decimal format.
