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

- It tries to mimic the same naming convention as React.js's class components (e.g. `render`)
- Built in components start with lower case letters
- Custom components start with upper case letters
- The main entry point is `GUI.render()`. Only call it once in your entire app.
- The `<vStack />` borrows from SwiftUI, which puts all of its children from top to bottom
- The `$gap` property (prop) is a style prop, applying 10 pixels of space between each of children.

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

| Property Name    | Property Type                                                                                                         | Examples                                     | Description                                                                                                                                                                 |
| ---------------- | --------------------------------------------------------------------------------------------------------------------- | -------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| $backgroundColor | `string`                                                                                                              | `#aabbcc`, `"#00ff00ff"`, `"tomato"`         | Supported color types are: HTML Colors or hex colors with or without an alpha value                                                                                         |
| $padding         | `integer` \| `{horizontal?: number; vertical?: number; left?: number; right?: number; top?: number; bottom?: number}` | `34`, `{vertical: 12}`, `{top: 6, left: 12}` | If passed a single number, padding applies to **all** sides, otherwise only what is specified. Numer is a 16bit integer. If this doesn't suffice, you are truly a maniac^^. |
| $fontSize        | `unsigned integer`                                                                                                    | `0` \| `100` \| `12`                         | Font size is generally thought of as `x pixels tall`. Default is 12 (pixels tall).                                                                                          |
| $color           | `string`                                                                                                              | `#aabbcc`, `"#00ff00ff"`, `"tomato"`         | Applied to `<text>`. Supported color types are: HTML Colors or hex colors with or without an alpha value.                                                                   |
| $letterSpacing   | `unsigned integer`                                                                                                    | `0` \| `1234`                                | Results in **horizontal** whitespace between the individual characters.                                                                                                     |

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
- `$padding`

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
- `$padding`
- `$color`
- `$fontSize`
- `$letterSpacing`

### Built in Event Listeners

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

##### Event Props

- `layerX` x position in pixels relative to the window.
- `layerY` y position in pixels relative to the window.

- `altKey` boolean indicating if the **alt key** was pressed.
- `ctrlKey` boolean indicating if the **control key** was pressed.
- `shiftKey` boolean indicating if the **shift key** was pressed.
