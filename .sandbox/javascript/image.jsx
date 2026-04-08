/**
 * This program will render 3 images with a different filter.
 * It will display them side by side. Differences are minimal, but noticable.
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
        return <hStack $gap={5}>
            <img data={this.data} filter="nearestNeighbor" />
            <img data={this.data} filter="bilinear" />
            <img data={this.data} />
        </hStack>
    }
}

GUI.render(<MyImage />, {
    width: 620,
    height: 210,
})