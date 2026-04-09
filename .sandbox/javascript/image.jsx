/**
 * This program will render 3 images with a different filter.
 * It will display them side by side. Differences are minimal, but noticable.
 */
import * as GUI from "GUI"

class MyImage {
    data;
    fetchImage() {
        fetch("https://picsum.photos/200")
            .then(res => res.blob())
            .then(data => { this.data = data })
            .catch(console.error)
    }

    constructor() {
        this.fetchImage()
    }

    render() {
        return <hStack $gap={5} onMouseUp={() => this.fetchImage()}>
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