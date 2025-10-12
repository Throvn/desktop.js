/**
 * This program will load different fonts (located in the assets/fonts dir).
 * It will display them using sample text, where each line should be rendered in a different font.
 */
import * as GUI from "GUI"

GUI.render(<vStack>
    <spacer />
    <text>This is the default font!</text>
    <spacer />
    <text>This is Roboto Regular!</text>
    <spacer />
</vStack>)