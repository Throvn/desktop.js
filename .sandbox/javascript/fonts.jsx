/**
 * This program will load different fonts (located in the assets/fonts dir).
 * It will display them using sample text, where each line should be rendered in a different font.
 */
import * as GUI from "GUI"

GUI.render(<vStack>
    <spacer />
    <text $fontFace="default">This is the default font!</text>
    <spacer />
    <text $fontFace="Roboto-Regular.ttf">This is Roboto Regular!</text>
    <spacer />
    <text $fontFace="Monaspace-Krypton.ttf">This is the Krypton font!</text>
    <spacer />
</vStack>)