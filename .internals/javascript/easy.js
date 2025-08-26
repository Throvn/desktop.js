import * as GUI from "GUI";

class Test {
    render = () => {
        return GUI.createElement("text", null, "cool");
    }
}
const test = GUI.createElement(Test, null);
console.log(test);
console.log(JSON.stringify(GUI.createElement("test", null, "b")));

GUI.render(test);