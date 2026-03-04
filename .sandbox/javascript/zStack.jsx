/**
 * Should render "Foreground" on top of "Background"
 */

import * as GUI from "GUI"

GUI.render(<hStack $backgroundColor="red">
    <hStack $backgroundColor="blue">
        <zStack>
        <text>
            This
        </text>
        <text>
            Is
        </text>
        <text>
            A
        </text>
        </zStack>
    </hStack>
</hStack>)