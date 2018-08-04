# Generate font from image
from PIL import Image

with open ("../src/mc_trs80font.c", "w+") as outFile:
    outFile.write("// TRS80 Font\n")
    outFile.write("// Autogenerated file - see GenFont.py\n")
    outFile.write("unsigned char pTRS80Font[] = {\n")

    pngFile = Image.open("TRS80Font.PNG")
    print(pngFile.size)
    pngData = pngFile.load()
    print(pngData[7,7])

    borderWidth = 4
    pixWidth = 2
    pixHeight = 3
    charWidthPix = 8
    charHeightPix = 8
    charCellWidth = 24
    charCellHeight = 72

    for rowChar in range(8):
        for colChar in range(32):
            outFile.write("// Char code 0x{:02x}\n".format(rowChar*32+colChar)) 
            for rowPix in range(charHeightPix):
                for colPix in range(charWidthPix):
                    pixX = colChar*charCellWidth + colPix*pixWidth + borderWidth
                    pixY = rowChar*charCellHeight + rowPix*pixHeight + borderWidth
                    #print(rowChar, colChar, rowPix, colPix, pixX, pixY)
                    pix = pngData[pixX, pixY]
                    outFile.write("0xff," if pix[1] != 0 else "0x00,")
                outFile.write("\n")
            outFile.write("\n")        
    outFile.write("};\n");
