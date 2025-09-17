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
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
var __generator = (this && this.__generator) || function (thisArg, body) {
    var _ = { label: 0, sent: function() { if (t[0] & 1) throw t[1]; return t[1]; }, trys: [], ops: [] }, f, y, t, g;
    return g = { next: verb(0), "throw": verb(1), "return": verb(2) }, typeof Symbol === "function" && (g[Symbol.iterator] = function() { return this; }), g;
    function verb(n) { return function (v) { return step([n, v]); }; }
    function step(op) {
        if (f) throw new TypeError("Generator is already executing.");
        while (_) try {
            if (f = 1, y && (t = op[0] & 2 ? y["return"] : op[0] ? y["throw"] || ((t = y["return"]) && t.call(y), 0) : y.next) && !(t = t.call(y, op[1])).done) return t;
            if (y = 0, t) op = [op[0] & 2, t.value];
            switch (op[0]) {
                case 0: case 1: t = op; break;
                case 4: _.label++; return { value: op[1], done: false };
                case 5: _.label++; y = op[1]; op = [0]; continue;
                case 7: op = _.ops.pop(); _.trys.pop(); continue;
                default:
                    if (!(t = _.trys, t = t.length > 0 && t[t.length - 1]) && (op[0] === 6 || op[0] === 2)) { _ = 0; continue; }
                    if (op[0] === 3 && (!t || (op[1] > t[0] && op[1] < t[3]))) { _.label = op[1]; break; }
                    if (op[0] === 6 && _.label < t[1]) { _.label = t[1]; t = op; break; }
                    if (t && _.label < t[2]) { _.label = t[2]; _.ops.push(op); break; }
                    if (t[2]) _.ops.pop();
                    _.trys.pop(); continue;
            }
            op = body.call(thisArg, _);
        } catch (e) { op = [6, e]; y = 0; } finally { f = t = 0; }
        if (op[0] & 5) throw op[1]; return { value: op[0] ? op[1] : void 0, done: true };
    }
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
        this.interval = setInterval(function () {
            _this.color = _this.color === "red" ? "blue" : "red";
        }, 1000);
    }
    GBText.prototype.render = function () {
        var _this = this;
        return GUI.createElement("group", { "$color": "#333960", "$lineHeight": 24 * 2, onMouseOver: this.onMouseOver },
            GUI.createElement("text", null, "Throvn's"),
            GUI.createElement("text", { "$fontSize": 24, "$color": this.color, onMouseDown: (function (e) {
                    clearInterval(_this.interval);
                }) }, this.name),
            GUI.createElement("text", { "$fontSize": 8 }, "TM"));
    };
    return GBText;
}());
function fetchImage() {
    var _a;
    return __awaiter(this, void 0, void 0, function () {
        var r, b, blob;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0:
                    console.log("fetching image");
                    return [4 /*yield*/, fetch('https://picsum.photos/200/300.jpg')];
                case 1:
                    r = _b.sent();
                    return [4 /*yield*/, r.blob()];
                case 2:
                    b = _b.sent();
                    blob = new Blob([b], {
                        type: (_a = r.headers.get('content-type')) !== null && _a !== void 0 ? _a : ''
                    });
                    console.log(blob.type);
                    return [2 /*return*/, blob];
            }
        });
    });
}
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
            console.log("test", event);
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
        fetchImage()
            .then(function (b) { return __awaiter(_this, void 0, void 0, function () {
            var arrayBuffer, buffer;
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4 /*yield*/, b.arrayBuffer()];
                    case 1:
                        arrayBuffer = _a.sent();
                        buffer = new Uint8Array(arrayBuffer);
                        // setTimeout(() => {
                        this.imageData = b;
                        return [2 /*return*/];
                }
            });
        }); });
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
                top: 10
            }, onMouseOver: this.handleMouseMove, "$width": 350 },
            GUI.createElement("spacer", null),
            GUI.createElement("vStack", null,
                "" + this.tick,
                GUI.createElement("hStack", { "$backgroundColor": "#4d4a55", "$padding": {
                        horizontal: 48,
                        vertical: 20
                    }, "$borderRadius": {
                        top: 8 + this.tick,
                        bottomLeft: 8,
                        bottomRight: 32
                    } },
                    GUI.createElement("vStack", { "$borderRadius": 10.5 }, grid)),
                GUI.createElement("hStack", { "$gap": 5, onMouseUp: function () {
                        _this.hideText = !_this.hideText;
                        console.log("hStack Mouse Up");
                    } },
                    !this.hideText ? GUI.createElement(GBText, null) : GUI.createElement("text", null, "Hidden"),
                    GUI.createElement("spacer", null),
                    GUI.createElement("img", { "$height": 100, data: this.imageData, onMouseUp: function (e) {
                            // e.stopPropagation();
                            console.log("Hidden Text Mouse Up");
                        }, onMouseOver: function (e) {
                            // e.stopPropagation();
                            var isEvent = e instanceof Event;
                            console.log("Image mouse over!", isEvent);
                        } },
                        GUI.createElement("text", { "$backgroundColor": "green", "$height": 100 }, "This image is not yet rendered! Lorem ipsum dolor sit amet consectetur adipisicing elit. A sapiente quibusdam consequuntur quo explicabo vero blanditiis consectetur ratione laborum, sint dicta reprehenderit aliquam quia quod delectus illo perferendis ipsa debitis?")),
                    this.hideText ? 'Hideen' : GUI.createElement("img", { "$height": 100, data: this.imageData, onMouseUp: function (e) {
                            // e.stopPropagation();
                            console.log("Hidden Text Mouse Up");
                        }, onMouseOver: function (e) {
                            _this.hideText = true;
                            // e.stopPropagation();
                            var isEvent = e instanceof Event;
                            console.log("Image mouse over!", isEvent);
                        } },
                        GUI.createElement("text", { "$backgroundColor": "green", "$height": 100 }, "This image is not yet rendered! Lorem ipsum dolor sit amet consectetur adipisicing elit. A sapiente quibusdam consequuntur quo explicabo vero blanditiis consectetur ratione laborum, sint dicta reprehenderit aliquam quia quod delectus illo perferendis ipsa debitis?")))),
            GUI.createElement("spacer", null),
            GUI.createElement("spacer", null),
            GUI.createElement("spacer", null)));
    };
    return GameBoard;
}());
GUI.render(GUI.createElement(GameBoard, null), {
    width: 300,
    height: 500,
    resizable: false
});
