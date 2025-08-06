import * as GUI from "GUI";
import * as os from "os";
// import * as React from "./build/libgui.so"
console.log("JSX test started...");
console.log("HELLO", globalThis);
console.log("Bye");
// const test = "hello";
// ===============
// class MyComponent extends React.Component {
var MyComponent = /** @class */ (function () {
    function MyComponent(props) {
        var _this = this;
        this.layerX = -1;
        this.handleMouseMove = function (event) {
            print(event);
            _this.layerX = event.layerX;
        };
        this.props = props;
        print("MyComponent constructor was called!");
    }
    MyComponent.prototype.anotherFunc = function () {
        return true;
    };
    MyComponent.prototype.render = function () {
        // print("called render()")
        return (GUI.createElement("vStack", { "$backgroundColor": "green", onMouseOver: this.handleMouseMove },
            GUI.createElement("text", { id: "container", width: 120, "$letterSpacing": 1, "$padding": 20, "$backgroundColor": "red" },
                "... ",
                this.layerX)));
    };
    return MyComponent;
}());
var Player = /** @class */ (function () {
    function Player(x, y) {
        this.direction = {
            x: 1,
            y: 1
        };
        this.x = x;
        this.y = y;
    }
    Player.prototype.move = function () {
        this.x += this.direction.x;
        this.y += this.direction.y;
    };
    return Player;
}());
var player = new Player(0, 0);
// os.setTimeout(changeColor, 30000)
var field = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
var grid = field.map(function (v, i) { return GUI.createElement("hStack", null, field.map(function (v, j) { return (GUI.createElement("text", { "$lineHeight": 120, "$padding": 8, "$backgroundColor": i === player.x && j === player.y ? "red" : "black" }, " A ")); })); });
print(grid);
GUI.render((GUI.createElement("vStack", null,
    "Hello Bye What",
    grid)));
var gameLoop = function () {
    player.move();
    grid = field.map(function (v, i) { return GUI.createElement("hStack", null, field.map(function (v, j) { return (GUI.createElement("text", { "$lineHeight": 120, "$padding": 8, "$backgroundColor": i === player.x && j === player.y ? "red" : "black" }, " A ")); })); });
    print("Moved");
    os.setTimeout(gameLoop, 500);
};
// gameLoop()
var evt = { layerX: -1 };
GUI.render(GUI.createElement(MyComponent, null));
// print(test, JSON.stringify(new MyComponent().render()))
