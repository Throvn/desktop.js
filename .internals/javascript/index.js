var __assign = (this && this.__assign) || function () {
    __assign = Object.assign || function(t) {
        for (var s, i = 1, n = arguments.length; i < n; i++) {
            s = arguments[i];
            for (var p in s) if (Object.prototype.hasOwnProperty.call(s, p))
                t[p] = s[p];
        }
        return t;
    };
    return __assign.apply(this, arguments);
};
import * as GUI from "GUI";
var SIZE = 10;
var MOUSE_THRESHOLD = 5; // pixels needed to trigger direction change
var Player = /** @class */ (function () {
    function Player(x, y) {
        this.direction = { x: 1, y: 0 };
        this.body = [];
        this.body = [{ x: x, y: y }];
    }
    Player.prototype.move = function () {
        var head = {
            x: (this.body[0].x + this.direction.x + SIZE) % SIZE,
            y: (this.body[0].y + this.direction.y + SIZE) % SIZE
        };
        this.body.unshift(head);
        this.body.pop();
    };
    Player.prototype.grow = function () {
        var tail = this.body[this.body.length - 1];
        this.body.push(__assign({}, tail));
    };
    return Player;
}());
var GBText = /** @class */ (function () {
    function GBText(props) {
        var _this = this;
        this.name = "SNAKE BOY";
        this.index = 0;
        this.onMouseOver = props.onMouseOver || (function () { });
        this.color = "red";
        console.log("New GBText");
        setInterval(function () {
            _this.color = _this.color === "red" ? "blue" : "red";
        }, 1000);
    }
    GBText.prototype.render = function () {
        return GUI.createElement("group", { "$color": "#333960", "$lineHeight": 24 * 2, onMouseOver: this.onMouseOver },
            GUI.createElement("text", null, "Throvn's"),
            GUI.createElement("text", { "$fontSize": 24, "$backgroundColor": this.color }, this.name),
            GUI.createElement("text", { "$fontSize": 8 }, "TM"));
    };
    return GBText;
}());
var GameBoard = /** @class */ (function () {
    function GameBoard(props) {
        var _this = this;
        this.food = { x: 5, y: 5 };
        this.lastMousePos = { x: 0, y: 0 };
        this.tick = 0;
        this.lastUpdate = 0;
        this.gameLoop = function () {
            _this.tick++;
            // Move snake
            _this.player.move();
            // Self collision check
            var head = _this.player.body[0];
            if (_this.player.body.slice(1).some(function (s) { return s.x === head.x && s.y === head.y; })) {
                console.log("Game Over");
                _this.player = new Player(0, 0);
            }
            // Eat food
            if (_this.isEating()) {
                _this.player.grow();
                _this.spawnFood();
            }
        };
        this.handleMouseMove = function (event) {
            var mouseX = event.layerX;
            var mouseY = event.layerY;
            var dx = mouseX - _this.lastMousePos.x;
            var dy = mouseY - _this.lastMousePos.y;
            // Only update if movement exceeds threshold
            if (Math.abs(dx) >= MOUSE_THRESHOLD || Math.abs(dy) >= MOUSE_THRESHOLD) {
                if (Math.abs(dx) > Math.abs(dy)) {
                    _this.player.direction = { x: dx > 0 ? 1 : -1, y: 0 };
                }
                else if (dy !== 0) {
                    _this.player.direction = { x: 0, y: dy > 0 ? 1 : -1 };
                }
                _this.lastMousePos = { x: mouseX, y: mouseY };
            }
        };
        this.props = props;
        this.player = new Player(0, 0);
        this.hideText = false;
        setInterval(this.gameLoop, 500);
    }
    GameBoard.prototype.spawnFood = function () {
        var pos;
        do {
            pos = { x: Math.floor(Math.random() * SIZE), y: Math.floor(Math.random() * SIZE) };
        } while (this.player.body.some(function (s) { return s.x === pos.x && s.y === pos.y; }));
        this.food = pos;
    };
    GameBoard.prototype.isEating = function () {
        var head = this.player.body[0];
        return head.x === this.food.x && head.y === this.food.y;
    };
    GameBoard.prototype.render = function () {
        var _this = this;
        // Draw grid
        var grid = [];
        var _loop_1 = function (i) {
            var row = [];
            var _loop_2 = function (j) {
                var isSnake = this_1.player.body.some(function (s) { return s.x === j && s.y === i; });
                var isFood = this_1.food.x === j && this_1.food.y === i;
                row.push(GUI.createElement("text", { "$lineHeight": 3, "$padding": 8, "$backgroundColor": isSnake ? "red" : isFood ? "green" : "#6f8500", "$borderRadius": {
                        bottomLeft: 20
                    } }, " "));
            };
            for (var j = 0; j < SIZE; j++) {
                _loop_2(j);
            }
            grid.push(GUI.createElement("hStack", { "$borderRadius": 20 }, row));
        };
        var this_1 = this;
        for (var i = 0; i < SIZE; i++) {
            _loop_1(i);
        }
        return (GUI.createElement("vStack", { "$backgroundColor": "#dbd8cc", "$padding": 8, "$borderRadius": {
                top: 16
            }, onMouseOver: this.handleMouseMove },
            GUI.createElement("spacer", null),
            GUI.createElement("vStack", null,
                "" + this.tick,
                GUI.createElement("vStack", { "$backgroundColor": "#4d4a55", "$padding": {
                        horizontal: 48,
                        vertical: 20
                    }, "$borderRadius": {
                        top: 8,
                        bottomLeft: 8,
                        bottomRight: 32
                    } },
                    GUI.createElement("vStack", { "$borderRadius": 10.5 }, grid)),
                GUI.createElement("hStack", { "$gap": 5 },
                    !this.hideText ? GUI.createElement(GBText, { onMouseOver: function () { _this.hideText = true; } }) : GUI.createElement("text", { onMOuseOver: function () { return _this.hideText = false; } }, "Hidden"),
                    GUI.createElement("spacer", null))),
            GUI.createElement("spacer", null),
            GUI.createElement("spacer", null),
            GUI.createElement("spacer", null)));
    };
    return GameBoard;
}());
GUI.render(GUI.createElement(GameBoard, null));
// class SmallTest {
//     constructor() {
//     }
//     render() {
//         return (<hStack onMouseOver={(event) => {
//             console.log(event)
//         }}>
//             <group >
//                 <text>Test</text>
//                 <spacer />
//                 <text>Bye</text>
//             </group>
//         </hStack>);
//     }
// }
// GUI.render(<SmallTest />)
