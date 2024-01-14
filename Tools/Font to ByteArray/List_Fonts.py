import matplotlib.font_manager
from PIL import ImageFont

Font_List = []

# Iterate over all font files known to matplotlib
for filename in matplotlib.font_manager.findSystemFonts(): 
    # Avoid these two trouble makers - don't know why they are problematic
    if "Emoji" not in filename and "18030" not in filename:
        # Look up what PIL knows about the font
        font = ImageFont.FreeTypeFont(filename)
        name, weight = font.getname()
        Font_List.append({'Font' : str(name), 'Weight': str(weight), 'File': str(filename)})

for Font in sorted(Font_List, key=lambda d: d['Font']) :
    print("Font: " + str(Font['Font']).ljust(30) + "Weight: " + str(Font['Weight']).ljust(20) + "File: " + Font['File'])