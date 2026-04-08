/**
 * This program will render an image which will grow based on the parent.
 * Aka, it will always take up the same space as the window and scale accordingly.
 */
import * as GUI from "GUI"

class MyImage {
    data;
    constructor() {
        fetch("https://picsum.photos/200")
            .then(res => res.blob())
            .then(data => { this.data = data })
            .catch(console.error)
    }

    render() {
        return <img data={this.data} $width="grow" $height="grow" />;
    }
}

GUI.render(<MyImage />, {
    height: 200,
    width: 200,
})