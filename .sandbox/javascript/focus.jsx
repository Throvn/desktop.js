/**
 * This program should display two boxes (left to right).
 * If you click on the first one, it should have a blue background, because it's focused. 
 * If you click on the second one, it should have a blue background and the first one should have a gray background again.
 * The boxes also capture keyboard input and displays it.
 */

import * as GUI from "GUI"

class RedBox {
    text = ""
    constructor(props) {
        this.props = props
        this.color = "white"
    }

    render() {
        return <vStack
            $backgroundColor={this.hasFocus ? "red" : "gray"} 
            onFocus={() => {
                this.hasFocus = true
                console.log("Focus In", this.props.children)
            }}
            onBlur={() => {
                this.hasFocus = false
                console.log("Focus Out",this.props.children)
            }}
            onKeyPress={(e) => {
                this.text += e.key
                this.color = "green"
            }}
            >
            <text $fontFace="Roboto-Regular.ttf">{this.text || this.props.children}</text>
        </vStack>
    }
}

GUI.render(
    <hStack $gap={5} onFocus={() => console.log("HSTACK")}>
        <RedBox>1</RedBox>
        <RedBox>2</RedBox>
    </hStack>
)