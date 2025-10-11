/**
 * This program should display two boxes (left to right). If you click on the first one, it should have a red border, because it's focused. If you click on the second one, it should have a red border and the first one shouldn't have a border anymore.
 * The boxes also capture keyboard input.
 */

import * as GUI from "GUI"

class RedBox {
    constructor(props) {
        this.props = props
        this.color = "white"
    }

    render() {
        return <vStack $backgroundColor={this.color} onKeyPress={(e) => {
            this.text += e.key
            this.color = "tomato"
        }}>{this.text || this.props.children}</vStack>
    }
}

GUI.render(
    <hStack $gap={5}>
        <RedBox>1</RedBox>
        <RedBox>2</RedBox>
    </hStack>
)