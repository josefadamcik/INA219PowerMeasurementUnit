Import('env')
env.Replace(FUSESCMD="avrdude $UPLOADERFLAGS -e -Ulock:w:0xFF:m -Uhfuse:w:0xD7:m -Uefuse:w:0xFF:m -Ulfuse:w:0xF1:m")
