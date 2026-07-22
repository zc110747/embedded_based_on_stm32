#!/use/bin/python3

import wave
import numpy as np
import sys

def decode_wavtoarray(wavefile, textfile):
    
    #读取并解析wav文件
    f = wave.open(wavefile, 'rb' )
    params = f.getparams()
    nchannels, sampwidth, framerate, nframes = params[:4]
    
    print(f"channels:{nchannels}, width:{sampwidth}")
    
    Data_str = f.readframes(nframes)
    Data_num = np.frombuffer(Data_str,dtype=np.int16)

    datalist = list(Data_num);
    f.close()

    with open(textfile, 'w+') as outFile:
        outFile.write("#include <stdint.h>\n")
        outFile.write("uint16_t tonePCM_8KHz16bit[{0}] = {{\n".format(len(datalist)+1))
        for index, item in enumerate(datalist):
            item += 32768
            outFile.write(str(item))
            outFile.write(',')
            if index !=0 and index%16 == 0:
                outFile.write('\n')
        outFile.write('0')
        outFile.write('\n};\n')

decode_wavtoarray("music.wav", "voice.c")