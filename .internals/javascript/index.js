import * as GUI from "GUI";
console.log("JSX test started...");
console.log("HELLO", globalThis);
console.log("Bye");
var MyComponent = /** @class */ (function () {
    function MyComponent(props) {
        var _this = this;
        this.layerX = -1;
        this.handleMouseMove = function (event) {
            console.log(event);
            _this.layerX = event.layerX;
        };
        this.props = props;
        console.log("MyComponent constructor was called!");
    }
    MyComponent.prototype.anotherFunc = function () {
        return true;
    };
    MyComponent.prototype.render = function () {
        this.layerX++;
        var val = (GUI.createElement("vStack", { "$backgroundColor": "green", onMouseOver: this.handleMouseMove },
            "MyComponent",
            GUI.createElement("spacer", null),
            GUI.createElement("text", null, "Test"),
            GUI.createElement("vStack", { "$padding": this.layerX % 50 }, this.layerX)));
        console.log("VAL: ", val);
        return val;
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
var field = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
var grid = field.map(function (v, i) { return GUI.createElement("hStack", null, field.map(function (v, j) { return (GUI.createElement("text", { "$lineHeight": 120, "$padding": 8, "$backgroundColor": i === player.x && j === player.y ? "red" : "black" }, " A ")); })); });
console.log(grid);
// GUI.render((<vStack>
//     Hello
//     Bye
//     What
//     {grid}
// </vStack>))
var gameLoop = function () {
    player.move();
    grid = field.map(function (v, i) { return GUI.createElement("hStack", null, field.map(function (v, j) { return (GUI.createElement("text", { "$lineHeight": 120, "$padding": 8, "$backgroundColor": i === player.x && j === player.y ? "red" : "black" }, " A ")); })); });
    console.log("Moved");
    os.setTimeout(gameLoop, 500);
};
GUI.render(GUI.createElement(MyComponent, null, "This is another test"));
