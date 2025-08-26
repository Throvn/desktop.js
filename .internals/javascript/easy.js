import * as GUI from "GUI";

class Test {

    constructor(props) {
        this.props = props;
        this.i = 0;
        setInterval(() => this.i++, 1000);
    }
    render = () => {
        return GUI.createElement("text", null, "cool" + this.i);
    }
}
const test = GUI.createElement(Test, null);
console.log(test);
console.log(JSON.stringify(GUI.createElement("test", null, "b")));

GUI.render(test);