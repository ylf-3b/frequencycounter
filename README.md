# frequencycounter
frequencycounter with arduino UNO

A simple frequency counter to measure lightning with the litght sensor TSL 235R.
To use the hole measuring range from this sensor type, a prescaler, wich divide the frequence by ten, is needed.
For the using of a prescaler, a constant in the programm ist used and by default it is set to ten. For using
without any or with other prescaling factors, it can be set to it needs.
At lower frequencies, the frequence will not be measured, but calculated from the period time measuring result.
At higher frequncies at 20kHz or above, the period time will be calculated from the frequency measuring result.
The maximum possible frequency ist not known yet, but 80kHz is proved.
Every minute the results are send via USB/serial. They are also displayed at a LCD, wich uses the same pinning
from the "Fototimer".
