import re
import os
import sys
from fontTools import ttLib
from PIL import Image, ImageFont, ImageDraw
import argparse

VERSION = '2.1'

def main():

    parser = argparse.ArgumentParser()
    parser.add_argument('-f', '--font'      , type=str, help='Define Font to be processed. Name should include modifier like Bold or Italic.')
    parser.add_argument('-s', '--fontsize'  , type=int, help='Fontsize')
    parser.add_argument('-t', '--height'    , type=str, default='auto', help='Define output height in pixel. Auto will determine value by itself.')
    parser.add_argument('-w', '--width'     , type=str, default='auto', help='Set output width in pixel. Auto will determine value by itself.')
    Args = parser.parse_args()

    if len(sys.argv) == 1:
        parser.print_help()
        return(1)

    # Folder where to find Font-files (.ttf-files)
    # Currently these folder are adapted for Windows. Change here in case an other OS is used
    Font_Folders = ["C:\\Windows\\Fonts\\", "C:\\Users\\" + os.getlogin() + "\\AppData\\Local\\Microsoft\\Windows\\Fonts\\"]

    # Get information about the specific Font file to be used from the input font name
    Font_Filename, Font_Directory, Font_Absolute_Path = Get_Font_Filename(Args.font, Font_Folders)

    # If font file could not be found, output according message and exit the script
    if(Font_Filename == -1):
        print('No font with name: ' + Args.font +' found' )
        return(-1)

    # Determine the width and height of the characters in pixel. This is required for later Byte-Array generation
    Font_Size = Args.fontsize
    Output_Pixel_Height, Top_White_Offset = Get_Output_Pixel_Height(Args.height, Font_Absolute_Path, Font_Size)
    Output_Pixel_Width, Left_White_Offset = Get_Output_Pixel_Width(Args.width, Font_Absolute_Path, Font_Size)

    # If it does not exist, create the output folder for the different Font-Byte-Arrays
    Output_Folder = Get_Output_Folder()

    # Create a separate output folder for the specific font 
    Output_Font_Folder = Get_Font_Output_Folder(Args.font, Output_Folder)

    # Get the filename for the different outout files of this script without the file extension
    Filename_wo_Extension = Get_Font_Filename_Without_Extension(Args.font, Output_Pixel_Width, Output_Pixel_Height)

    # Create the JPG preview file of the foint
    Create_Preview_Image(Font_Absolute_Path, Font_Size, Output_Pixel_Width, Output_Pixel_Height, Output_Font_Folder, Filename_wo_Extension)

    # Finally, write the C-output file with the actual font as Byte-Array
    Create_C_File(Font_Absolute_Path, Font_Size, Output_Pixel_Width, Output_Pixel_Height, Output_Font_Folder, Filename_wo_Extension)
    


#---------------------------------------------------------------------------------------
# Generate C file
def Create_C_File(font_absolute_path, font_size, output_pixel_width, output_pixel_height, output_folder, filename_wo_extension):
    
    C_File_Pointer = open(output_folder + filename_wo_extension + ".c","w+")
    TXT_File_Pointer = open(output_folder + filename_wo_extension + ".txt","w+")

    Create_C_File_Header(C_File_Pointer, output_pixel_width, output_pixel_height, filename_wo_extension)
    
    for Character in Get_Character_List():
        Create_C_File_Write_Character(C_File_Pointer, Character, font_absolute_path, font_size, output_pixel_width, output_pixel_height, Character == Get_Character_List()[len(Get_Character_List()) - 1], TXT_File_Pointer)

        if(Character != Get_Character_List()[len(Get_Character_List()) - 1]):
            C_File_Pointer.write("\n")
        else:
            C_File_Pointer.write("\n};\n")
            C_File_Pointer.write("\n")

    C_File_Pointer.close()
    TXT_File_Pointer.close()
    print("File '" + filename_wo_extension + ".c' written")
    

def Create_C_File_Header(C_file_pointer, output_pixel_width, output_pixel_height, filename_wo_extension):
    C_file_pointer.write("#include <stdint.h>\n")
    C_file_pointer.write("\n")
    C_file_pointer.write("const uint8_t _" + filename_wo_extension + "[] = {\n")
    C_file_pointer.write("\t0x00, 0x00,\t// Size of zero indicates fixed width font, actual length is width * height\n")
    C_file_pointer.write("\t0x{:02x}".format(int(output_pixel_width))           + ",\t\t// Width\n")
    C_file_pointer.write("\t0x{:02x}".format(int(output_pixel_height))          + ",\t\t// Height\n")
    C_file_pointer.write("\t0x{:02x}".format(int(ord(Get_Character_List()[0])))	+ ",\t\t// First Char\n")
    C_file_pointer.write("\t0x{:02x}".format(len(Get_Character_List()))         + ",\t\t// Char Count\n")
    C_file_pointer.write("\n")
    C_file_pointer.write("\t// Fixed width; char width table not used !!!!\n")
    C_file_pointer.write("\n")
    C_file_pointer.write("\t// Font Data\n")

def Create_C_File_Write_Character(C_file_pointer, character, font_absolute_path, font_size, output_pixel_width, output_pixel_height, is_last, TXT_file_pointer):
    Threshold = 127

    PIL_Font = ImageFont.truetype(font_absolute_path, font_size)
    Image_Size = (output_pixel_width, output_pixel_height)

    Image_Character = Image.new('1', Image_Size, color=255)
    Image_Character_Draw = ImageDraw.Draw(Image_Character)
    Image_Character_Draw.text((0, 0), character, font=PIL_Font)

    C_file_pointer.write("\t")

    for y in range(0, int((Image_Size[1]+7)/8)):
        for x in range(0, Image_Size[0]):
            if(y > 0 or x > 0):
                C_file_pointer.write(", ")

            dot_byte = 0
            for k in range(8):
                if((y * 8 + k) < Image_Size[1]):
                    Pixel_Value = Image_Character.getpixel((x, (y * 8 + k)))
                else:
                    Pixel_Value = Threshold
                if(Pixel_Value < Threshold):
                    dot_byte = dot_byte + 2**k
            C_file_pointer.write("0x{:02x}".format(int(dot_byte)))

    if(is_last == False):
        C_file_pointer.write(",")

    if(character == "\\"):
        C_file_pointer.write("\t// \"" + character + "\"")
    else:
        C_file_pointer.write("\t// " + character)


    for y in range(0, Image_Size[1]):
        for x in range(0, Image_Size[0]):
            if(Image_Character.getpixel((x, y)) < 127):
                TXT_file_pointer.write("#")
            else:
                TXT_file_pointer.write("_")
            
        TXT_file_pointer.write("\n")

    TXT_file_pointer.write("\n")


#---------------------------------------------------------------------------------------
# Generate Preview Image with all characters contained by the font
def Create_Preview_Image(font_absolute_path, font_size, output_pixel_width, output_pixel_height, output_folder, filename_wo_extension):
    PIL_Font = ImageFont.truetype(font_absolute_path, font_size)
    Image_Size = (len(Get_Character_List())*output_pixel_width, output_pixel_height)

    Image_Preview = Image.new('L', Image_Size, color=255)
    Image_Preview_Draw = ImageDraw.Draw(Image_Preview)

    for Character in Get_Character_List():
        Character_Index = Get_Character_List().index(Character)
        Character_X = Character_Index * output_pixel_width

        if(Character_Index % 2 == 0):
            Image_Preview_Draw.rectangle((Character_X, 0, Character_X + output_pixel_width-1, output_pixel_height), 200)

        Image_Preview_Draw.text((Character_X, 0), Character, font=PIL_Font)

    Image_Preview.save(output_folder + filename_wo_extension + ".png")


#---------------------------------------------------------------------------------------
# Search all Folders and check for filenames of Font Names, required by PIL TTF Font handler
def Get_Font_Filename (font_name, font_folders):
    All_Font_Files = []

    for font_folder in font_folders:
        for dirpath, dirnames, filenames in os.walk(font_folder):
            for filename in [f for f in filenames if f.endswith(".ttf")]:
                Font_File = {'dir': dirpath, 'filename': filename}
                All_Font_Files.append(Font_File)

    for Font_File in All_Font_Files:
        Font_Absolute_Filename = os.path.join(Font_File['dir'], Font_File['filename'])
        Font = Get_Font_From_Absolute_Path(Font_Absolute_Filename)

        if(font_name == Font):
            Target_Font_File        = Font_File['filename']
            Target_Font_Directory   = Font_File['dir']
            return Target_Font_File, Target_Font_Directory, Font_Absolute_Filename

    return -1, -1, -1


#-----------------------------------------------------------------------------------------
# Process the Height input argument and find the proper height, if option is set to 'auto'
def Get_Output_Pixel_Height(height_argument, font_absolute_path, font_size):
    if(height_argument != 'auto'):
        return int(re.findall('[0-9]+', height_argument)[0])
    
    PIL_Font = ImageFont.truetype(font_absolute_path, font_size)
    Image_Size = (2*font_size, 2*font_size)

    Top_Offset = 2*font_size
    Bot_Offset = 0

    Image_Test = Image.new('1', Image_Size, color=255)
    Image_Draw = ImageDraw.Draw(Image_Test)
    for Character in Get_Character_List():
        Image_Draw.text((0, 0), Character, font=PIL_Font)

    for Top in range(0, Image_Size[1]):
        for x in range(0, Image_Size[0]):
            if(Image_Test.getpixel((x, Top)) < 127):
                if(Top < Top_Offset):
                    Top_Offset = Top
                break

    for Bot in reversed(range(0, Image_Size[1])):
        for x in range(0, Image_Size[0]):
            if(Image_Test.getpixel((x, Bot)) < 127):
                if(Bot > Bot_Offset):
                    Bot_Offset = Bot
                break
        
    return Bot_Offset+1, Top_Offset


#-----------------------------------------------------------------------------------------
# Process the Width input argument and find the proper height, if option is set to 'auto'
def Get_Output_Pixel_Width(width_argument, font_absolute_path, font_size):
    if(width_argument != 'auto'):
        return int(re.findall('[0-9]+', width_argument)[0])
    
    PIL_Font = ImageFont.truetype(font_absolute_path, font_size)
    Image_Size = (2*font_size, 2*font_size)

    Left_Offset = 2*font_size
    Right_Offset = 0

    Image_Test = Image.new('1', Image_Size, color=255)
    Image_Draw = ImageDraw.Draw(Image_Test)
    for Character in Get_Character_List():
        Image_Draw.text((0, 0), Character, font=PIL_Font)
      
    for Left in range(0, Image_Size[0]):
        for y in range(0, Image_Size[1]):
            if(Image_Test.getpixel((Left, y)) < 127):
                if(Left < Left_Offset):
                    Left_Offset = Left
                break

    for Right in reversed(range(0, Image_Size[0])):
        for y in range(0, Image_Size[1]):
            if(Image_Test.getpixel((Right, y)) < 127):
                if(Right > Right_Offset):
                    Right_Offset = Right
                break

    return Right_Offset, Left_Offset


#-----------------------------------------------------------------------------------------
# Get Output folder and create it if necessary
def Get_Output_Folder():
    Output_Folder = "Fonts/"

    if not (os.path.exists(Output_Folder)):
        os.mkdir(Output_Folder)

    return Output_Folder


#-----------------------------------------------------------------------------------------
# Get Output folder for the specific font and create it if necessary
def Get_Font_Output_Folder(font_name, output_folder):
    Font_Output_Folder = output_folder + font_name + "/"

    if not (os.path.exists(Font_Output_Folder)):
        os.mkdir(Font_Output_Folder)

    return Font_Output_Folder


#-----------------------------------------------------------------------------------------
# Get Font output filename without ending
def Get_Font_Filename_Without_Extension(font_name, width, height):
    return "Font_" + font_name.replace(" ", "_") + '_' + str(width) + 'x' + str(height)


#-----------------------------------------------------------------------------------------
# Generate and return a list of characters to convert
def Get_Character_List():
    Character_List = []
    Character_Line = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
    [Character_List.append(x) for x in Character_Line if x not in Character_List]
    
    return Character_List


#-----------------------------------------------------------------------------------------
# Get the actual font from a TTF-file
def Get_Font_From_Absolute_Path(font_absolute_path):
    Font = ttLib.TTFont(font_absolute_path)
    Font_Info = Font['name'].names[4].string
    Font = Font_Info.decode('ascii', errors ='replace')
    Font = re.sub('\x00', '', Font)

    return Font


if (__name__ == '__main__'):
    main()