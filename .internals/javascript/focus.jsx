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
            $backgroundColor={this.hasFocus ? "blue" : "gray"} 
            onFocus={(e) => {
                this.hasFocus = true
                console.log("Focus In", this.props.children)
            }}
            onBlur={(e) => {
                this.hasFocus = false
                console.log("Focus Out",this.props.children)
            }}
            onKeyPress={(e) => {
                this.text += e.key
                this.color = "tomato"
            }}
            >
            {this.text || this.props.children}
        </vStack>
    }
}

GUI.render(
    <hStack $gap={5}>
        <RedBox>1</RedBox>
        <RedBox>2</RedBox>
    </hStack>
)