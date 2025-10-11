# Desktop.js

Desktop.js is a highly opinionated UI framework to develop small, fast desktop applications, written in JavaScript.
It has a reactive syntax, is **very** easy to learn, a joy to write, and works with your existing tools.

## Value Proposition

Unlike Electron, it is about 10x smaller and you don't need to handle message passing between front- and backend.
Unlike Tauri, it does not depend on platform specific quirks and you don't need to deal with rust.

**How is this possible?**

We translate your JSX into native draw calls, ensuring platform independent results.

Also, to decrease bundle size, we don't rely on heavy runtimes like node, deno or bun but instead use QuickJS.
This comes with its own list of pros and cons but as development progresses, we plan on getting more and more [WinterTC](https://min-common-api.proposal.wintertc.org/) compliant.

Since there is a 90% chance that the app you have in mind is a CRUD app anyway, you can already give it a try.

The plan is to start with exceptionally well filesystem support and a feature complete `fetch()` implementation.
From there on we would like to focus on process interaction (spawning other programs, etc). If you have a function which is keeping you from using Desktop.js, let me know by opening an issue. If you have the ability to solve it, even better, as contributions are very welcome!

## Documentation

You can think of this framework as a proud child of React.js and SwiftUI.

The UI has a component system with naming conventions closely matching web standards.

Let's look at a very simple example:

```js
class MyComponent {
  render() {
    return <vStack $gap={10}>Hello World!</vStack>;
  }
}

GUI.render(<MyComponent />);
```

We can see:

- It tries to mimic the same naming convention as React.js's class components (e.g. `render`).
- **Built in** components start with lower case letters.
- **Custom** components start with upper case letters.
- The main entry point is `GUI.render()`. Only call it once in your entire app.
- The `<vStack />` borrows from SwiftUI, which puts all of its children from top to bottom.
- The `$gap` property (prop) is a style prop, applying 10 pixels of space between each of the children.

### Styling

Probably, the biggest difference to what you are used to know is that you don't style using CSS.

I've done some extensive research on all the different ways of styling a UI and came to the conclusion that each approach receives an equal amount of hate.
So I've decided to again draw inspiration from SwiftUIs View Modifiers.
For React folks: you style your components using props.

Before you start screaming, here's a hot take: You've already done it before anyway.
Tailwind, inline styles and the style prop is all the same. Having each style as a prop is not much different.

As a convention, styles start with a dollar sign (`$`).
This has two advantages:

1. you can differentiate them more easily from the rest of your props.
2. it can be used for an autocomplete trigger in your IDE. I don't have tool support yet though, feel free to start one though.

### Built in Style Properties

> **Note:** If you are here the first time, you might want to read the [Built in Components](#built-in-components) section first.

| Property Name    | Property Type                                                                                                                                                | Examples                                        | Description                                                                                                                                                                                                               |
| ---------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------ | ----------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| $backgroundColor | `string`                                                                                                                                                     | `"#aabbcc"`, `"#00ff00ff"`, `"tomato"`          | Supported color types are: HTML Colors or hex colors with or without an alpha value                                                                                                                                       |
| $padding         | `integer` \| `{horizontal?: number; vertical?: number; left?: number; right?: number; top?: number; bottom?: number}`                                        | `34`, `{vertical: 12}`, `{top: 6, left: 12}`    | If passed a single number, padding applies to **all** sides, otherwise only what is specified. Numer is a 16bit integer. If this doesn't suffice, you are truly a maniac^^.                                               |
| $fontSize        | `unsigned integer`                                                                                                                                           | `0` \| `100` \| `12`                            | Font size is generally thought of as `x pixels tall`. Default is 12 (pixels tall).                                                                                                                                        |
| $color           | `string`                                                                                                                                                     | `#aabbcc`, `"#00ff00ff"`, `"tomato"`            | Applied to `<text>`. Supported color types are: HTML Colors or hex colors with or without an alpha value.                                                                                                                 |
| $letterSpacing   | `unsigned integer`                                                                                                                                           | `0` \| `1234`                                   | Results in **horizontal** whitespace between the individual characters.                                                                                                                                                   |
| $gap             | `unsigned integer`                                                                                                                                           | `0` \| `8`                                      | Adds x `px` of spacing between the child elements of a stack.                                                                                                                                                             |
| $borderRadius    | `number` \| `{top?: number, bottom?: number, left?: number, right?: number, topLeft?: number, topRight?: number, bottomLeft?: number, bottomRight?: number}` | `{top: 12, bottomRight: 8, topLeft: 2}` \| `12` | Determines how rounded the corners of a component are. Union radii (like top or bottom) are evaluated first, which makes it possible to overwrite part of the values through more fine grained properties like `topLeft`. |
| $lineHeight      | `unsigned integer`                                                                                                                                           | `12` \| `15`                                    | Determines how tall the text should be. Default is 12 (just like the font size).                                                                                                                                          |
| $width           | `unsigned integer` \| `string`                                                                                                                               | `24` \| `7` \| `2` \| `"grow"` \| `"fit"`       | Give the component a **fixed** width.                                                                                                                                                                                     |
| $height          | `unsigned integer` \| `string`                                                                                                                               | `24` \| `7` \| `2` \| `"grow"` \| `"fit"`       | Give the component a **fixed** height.                                                                                                                                                                                    |

### Built in Components

#### `<group ...props/>`

A container component which applies all of its props to each child individually. E.g. useful for styling list items.

- If the same property was already defined on the child, the property is **NOT** applied. As props closer to the component always take precedent.
- If a child component doesn't specify the style, it is ignored by the child component.
- It doesn't contribute an element to the view hierarchy. (Similar to [React.js Fragment](https://react.dev/reference/react/Fragment))

##### Example

When you have the following code:

```html
<vStack>
  <group $backgroundColor="black">
    <text>First Text</text>
    <text>Second Text</text>
  </group>
</vStack>
```

You can think of it as:

```html
<vStack>
  <text $backgroundColor="black">First Text</text>
  <text $backgroundColor="black">Second Text</text>
</vStack>
```

#### `<vStack>`

This container component orders its child components **vertically** (from top to bottom).
Each child is centered automatically. To change the position of children use [`<spacer />`s](#spacer-).

##### Style Props

- `$backgroundColor`
- `$borderRadius`
- `$padding`
- `$gap`
- `$width` Additionally to pixel values, you can supply `"grow"` or `"fit"`.
- `$height` Additionally to pixel values, you can supply `"grow"` or `"fit"`.

#### `<hStack>`

This container component orders its child components **horizontally** (from left to right).
For more usage details & style props see [`<vStack>`](#vstack).

#### `<spacer />`

A component which takes up all available space inside of its parent component.
Use it to position children of `<hStack>` and `<vStack>`.

This component doesn't take any props.

#### `<text>`

A component to style text. All of its children **NEED** to be strings, otherwise, _undefined_ will be printed on the screen.
The default font size is 12px tall and the default color is black.

##### Style Props

- `$backgroundColor`
- `$borderRadius`
- `$padding`
- `$color`
- `$fontSize`
- `$letterSpacing`
- `$lineHeight`

#### `<img data={Blob}>`

A component to display an image. It's children are displayed only if the the supplied `data` prop is not a valid `Blob` object, functioning as a placeholder.

If the `$width` and or `$height` attributes are set before the image loaded, the placeholder will receive the same dimensions.

##### Supported MIME types

- `image/bmp`
- `image/gif` Only shows the first frame. Not moving images!
- `image/jpeg`
- `image/png`
- `image/psd`

##### Example

**Note**: A correct file type of the `Blob` is required! The current txiki.js version [has a bug](https://github.com/saghul/txiki.js/pull/723) where the `Blob` from the fetch response does not yet have a file type. If this is the case, you need to set it manually (easiest by wrapping the returned blob in a new one where you set the type in the initialization options).

```jsx
const r = await fetch("https://picsum.photos/200/300.jpg");
const blob = await r.blob();

<img $width={150} $height={150} data={blob}>
  The image is not fully loaded yet!
</img>;
```

##### Style Props

- `$backgroundColor` Only applied to the children.
- `$width` Only pixel values (numbers) have an effect.
- `$height` Only pixel values (numbers) have an effect.

### Built in Event Listeners

#### `onMouseUp`

The mouse down event is called once a mouse button was pressed.
It tries to mimic the [MouseUp JS event](https://developer.mozilla.org/en-US/docs/Web/API/Element/mouseup_event).

##### Example

```jsx
<text
  onMouseUp={(event) => {
    console.log("Mouse UP Event Called!");
  }}
></text>
```

##### Event Props

- `button` the id of the pressed mouse button (0 = left, 1 = right, 2 = middle).
- `layerX` x position in pixels relative to the window.
- `layerY` y position in pixels relative to the window.
- `altKey` boolean indicating if the **alt key** was pressed.
- `ctrlKey` boolean indicating if the **control key** was pressed.
- `shiftKey` boolean indicating if the **shift key** was pressed

#### `onMouseDown`

The mouse down event is called once a mouse button was pressed.
It tries to mimic the [MouseDown JS event](https://developer.mozilla.org/en-US/docs/Web/API/Element/mousedown_event).

##### Example

```jsx
<text
  onMouseDown={(event) => {
    console.log("Mouse DOWN Event Called!");
  }}
></text>
```

##### Event Props

- `button` the id of the pressed mouse button (0 = left, 1 = right, 2 = middle).
- `layerX` x position in pixels relative to the window.
- `layerY` y position in pixels relative to the window.
- `altKey` boolean indicating if the **alt key** was pressed.
- `ctrlKey` boolean indicating if the **control key** was pressed.
- `shiftKey` boolean indicating if the **shift key** was pressed.

#### `onMouseOver`

The mouse over event is called everytime the mouse moves on top of the element.
It is called once per frame and tries to mimic the [MouseOver JS event](https://developer.mozilla.org/en-US/docs/Web/API/Element/mouseover_event).

##### Example

```jsx
<text
  onMouseOver={(event) => {
    console.log("Mouse OVER Event Called!");
  }}
></text>
```

#### `onKeyPress`

This event fires when the element is the current focus and a key was pressed.

The values of the `key` property are a subset of the list you can find [here](https://developer.mozilla.org/en-US/docs/Web/API/UI_Events/Keyboard_event_key_values#browser_control_keys).

##### Event Props

- `code` integer determining the pressed key code
- `key` string representation of the pressed key. E.g. 'a' or 'A' or 'Backspace'...
- `altKey` boolean indicating if the **alt key** was pressed.
- `ctrlKey` boolean indicating if the **control key** was pressed.
- `shiftKey` boolean indicating if the **shift key** was pressed.

##### Example

```jsx
<vStack
  onKeyPress={(event) => {
    console.log(
      "The key '" +
        event.key +
        "' with the code '" +
        event.code +
        "' was pressed!"
    );
  }}
></vStack>
```

#### `onFocus`

The focus event is called when an element **gains** focus. This predominantly happens when the element is clicked. A focused element can receive e.g. keyboard input.

The event does **not** bubble.

If the focused element loses focus, the `onBlur` will tell you.

##### Example

```jsx
<vStack
  onFocus={() => {
    console.log("This element received focus!");
  }}
></vStack>
```

#### `onBlur`

Think of this event as the opposite to `onFocus`.
The blur event is called when an element **loses** focus.

The event does **not** bubble.

##### Example

```jsx
<vStack
  onBlur={() => {
    console.log("ANOTHER element just received focus!");
  }}
></vStack>
```

##### Event Props

- `layerX` x position in pixels relative to the window.
- `layerY` y position in pixels relative to the window.
- `altKey` boolean indicating if the **alt key** was pressed.
- `ctrlKey` boolean indicating if the **control key** was pressed.
- `shiftKey` boolean indicating if the **shift key** was pressed.

## Build & Run

### Run

If you are on macOS, you can use the run script.
For this to work, you need to have the typescript compiler (`tsc`) and `clang` installed.
The script will first look for a `.internals/javascript/index.jsx` file and compile this with the correct flags to an `index.js` file.
It will then run the `make run` command, which creates a new binary, runs it and loads the code at `.internals/javascript/index.js`.

```
./run.sh
```
