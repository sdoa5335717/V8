// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

"use strict";

// If true, prints all messages sent and received by inspector.
const printProtocolMessages = false;

// The active wrapper instance.
let activeWrapper = undefined;

// Receiver function called by inspector, delegating to active wrapper.
function receive(message) {
  activeWrapper.receiveMessage(message);
}

class DebugWrapper {
  constructor() {
    // Message dictionary storing {id, message} pairs.
    this.receivedMessages = new Map();

    // Each message dispatched by the Debug wrapper is assigned a unique number
    // using nextMessageId.
    this.nextMessageId = 0;

    // The listener method called on certain events.
    this.listener = undefined;

    // TODO(jgruber): Determine which of these are still required and possible.
    // Debug events which can occur in the V8 JavaScript engine.
    this.DebugEvent = { Break: 1,
                        Exception: 2,
                        NewFunction: 3,
                        BeforeCompile: 4,
                        AfterCompile: 5,
                        CompileError: 6,
                        AsyncTaskEvent: 7
                      };

    // The different types of steps.
    this.StepAction = { StepOut: 0,
                        StepNext: 1,
                        StepIn: 2,
                        StepFrame: 3,
                      };

    // The different types of scripts matching enum ScriptType in objects.h.
    this.ScriptType = { Native: 0,
                        Extension: 1,
                        Normal: 2,
                        Wasm: 3};

    // A copy of the scope types from runtime-debug.cc.
    // NOTE: these constants should be backward-compatible, so
    // add new ones to the end of this list.
    this.ScopeType = { Global:  0,
                       Local:   1,
                       With:    2,
                       Closure: 3,
                       Catch:   4,
                       Block:   5,
                       Script:  6,
                       Eval:    7,
                       Module:  8
                     };

    // Types of exceptions that can be broken upon.
    this.ExceptionBreak = { Caught : 0,
                            Uncaught: 1 };

    // The different types of breakpoint position alignments.
    // Must match BreakPositionAlignment in debug.h.
    this.BreakPositionAlignment = {
      Statement: 0,
      BreakPosition: 1
    };

    // The different script break point types.
    this.ScriptBreakPointType = { ScriptId: 0,
                                  ScriptName: 1,
                                  ScriptRegExp: 2 };

    // Store the current script id so we can skip corresponding break events.
    this.thisScriptId = %FunctionGetScriptId(receive);

    // Stores all set breakpoints.
    this.breakpoints = new Set();

    // Register as the active wrapper.
    assertTrue(activeWrapper === undefined);
    activeWrapper = this;
  }

  enable() { this.sendMessageForMethodChecked("Debugger.enable"); }
  disable() { this.sendMessageForMethodChecked("Debugger.disable"); }

  setListener(listener) { this.listener = listener; }

  stepOver() { this.sendMessageForMethodChecked("Debugger.stepOver"); }
  stepInto() { this.sendMessageForMethodChecked("Debugger.stepInto"); }
  stepOut() { this.sendMessageForMethodChecked("Debugger.stepOut"); }

  setBreakOnException()  {
    this.sendMessageForMethodChecked(
        "Debugger.setPauseOnExceptions", { state : "all" });
  }

  clearBreakOnException()  {
    const newState = this.isBreakOnUncaughtException() ? "uncaught" : "none";
    this.sendMessageForMethodChecked(
        "Debugger.setPauseOnExceptions", { state : newState });
  }

  isBreakOnException() {
    return !!%IsBreakOnException(this.ExceptionBreak.Caught);
  };

  setBreakOnUncaughtException()  {
    const newState = this.isBreakOnException() ? "all" : "uncaught";
    this.sendMessageForMethodChecked(
        "Debugger.setPauseOnExceptions", { state : newState });
  }

  clearBreakOnUncaughtException()  {
    const newState = this.isBreakOnException() ? "all" : "none";
    this.sendMessageForMethodChecked(
        "Debugger.setPauseOnExceptions", { state : newState });
  }

  isBreakOnUncaughtException() {
    return !!%IsBreakOnException(this.ExceptionBreak.Uncaught);
  };

  clearStepping() { %ClearStepping(); };

  // Returns the resulting breakpoint id.
  setBreakPoint(func, opt_line, opt_column, opt_condition) {
    assertTrue(%IsFunction(func));
    assertFalse(%FunctionIsAPIFunction(func));

    const scriptid = %FunctionGetScriptId(func);
    assertTrue(scriptid != -1);

    const offset = %FunctionGetScriptSourcePosition(func);
    const loc =
      %ScriptLocationFromLine2(scriptid, opt_line, opt_column, offset);
    return this.setBreakPointAtLocation(scriptid, loc, opt_condition);
  }

  setScriptBreakPoint(type, scriptid, opt_line, opt_column, opt_condition) {
    // Only sets by script id are supported for now.
    assertEquals(this.ScriptBreakPointType.ScriptId, type);
    return this.setScriptBreakPointById(scriptid, opt_line, opt_column,
                                        opt_condition);
  }

  setScriptBreakPointById(scriptid, opt_line, opt_column, opt_condition) {
    const loc = %ScriptLocationFromLine2(scriptid, opt_line, opt_column, 0);
    return this.setBreakPointAtLocation(scriptid, loc, opt_condition);
  }

  clearBreakPoint(breakid) {
    assertTrue(this.breakpoints.has(breakid));
    const {msgid, msg} = this.createMessage(
        "Debugger.removeBreakpoint", { breakpointId : breakid });
    this.sendMessage(msg);
    this.takeReplyChecked(msgid);
    this.breakpoints.delete(breakid);
  }

  clearAllBreakPoints() {
    for (let breakid of this.breakpoints) {
      this.clearBreakPoint(breakid);
    }
    this.breakpoints.clear();
  }

  showBreakPoints(f, opt_position_alignment) {
    if (!%IsFunction(f)) throw new Error("Not passed a Function");

    const source = %FunctionGetSourceCode(f);
    const offset = %FunctionGetScriptSourcePosition(f);
    const position_alignment = opt_position_alignment === undefined
        ? this.BreakPositionAlignment.Statement : opt_position_alignment;
    const locations = %GetBreakLocations(f, position_alignment);

    if (!locations) return source;

    locations.sort(function(x, y) { return x - y; });

    let result = "";
    let prev_pos = 0;
    let pos;

    for (var i = 0; i < locations.length; i++) {
      pos = locations[i] - offset;
      result += source.slice(prev_pos, pos);
      result += "[B" + i + "]";
      prev_pos = pos;
    }

    pos = source.length;
    result += source.substring(prev_pos, pos);

    return result;
  }

  debuggerFlags() {
    return { breakPointsActive :
                { setValue : (enabled) => this.setBreakPointsActive(enabled) }
           };
  }

  scripts() {
    // Collect all scripts in the heap.
    return %DebugGetLoadedScripts();
  }

  // Returns a Script object. If the parameter is a function the return value
  // is the script in which the function is defined. If the parameter is a
  // string the return value is the script for which the script name has that
  // string value.  If it is a regexp and there is a unique script whose name
  // matches we return that, otherwise undefined.
  findScript(func_or_script_name) {
    if (%IsFunction(func_or_script_name)) {
      return %FunctionGetScript(func_or_script_name);
    } else if (%IsRegExp(func_or_script_name)) {
      var scripts = this.scripts();
      var last_result = null;
      var result_count = 0;
      for (var i in scripts) {
        var script = scripts[i];
        if (func_or_script_name.test(script.name)) {
          last_result = script;
          result_count++;
        }
      }
      // Return the unique script matching the regexp.  If there are more
      // than one we don't return a value since there is no good way to
      // decide which one to return.  Returning a "random" one, say the
      // first, would introduce nondeterminism (or something close to it)
      // because the order is the heap iteration order.
      if (result_count == 1) {
        return last_result;
      } else {
        return undefined;
      }
    } else {
      return %GetScript(func_or_script_name);
    }
  }

  // Returns the script source. If the parameter is a function the return value
  // is the script source for the script in which the function is defined. If the
  // parameter is a string the return value is the script for which the script
  // name has that string value.
  scriptSource(func_or_script_name) {
    return this.findScript(func_or_script_name).source;
  };

  sourcePosition(f) {
    if (!%IsFunction(f)) throw new Error("Not passed a Function");
    return %FunctionGetScriptSourcePosition(f);
  };

  // Returns the character position in a script based on a line number and an
  // optional position within that line.
  findScriptSourcePosition(script, opt_line, opt_column) {
    var location = %ScriptLocationFromLine(script, opt_line, opt_column, 0);
    return location ? location.position : null;
  };

  findFunctionSourceLocation(func, opt_line, opt_column) {
    var script = %FunctionGetScript(func);
    var script_offset = %FunctionGetScriptSourcePosition(func);
    return %ScriptLocationFromLine(script, opt_line, opt_column, script_offset);
  }

  setBreakPointsActive(enabled) {
    const {msgid, msg} = this.createMessage(
        "Debugger.setBreakpointsActive", { active : enabled });
    this.sendMessage(msg);
    this.takeReplyChecked(msgid);
  }

  get LiveEdit() {
    const debugContext = %GetDebugContext();
    return debugContext.Debug.LiveEdit;
  }

  // --- Internal methods. -----------------------------------------------------

  getNextMessageId() {
    return this.nextMessageId++;
  }

  createMessage(method, params) {
    const id = this.getNextMessageId();
    const msg = JSON.stringify({
      id: id,
      method: method,
      params: params,
    });
    return { msgid : id, msg: msg };
  }

  receiveMessage(message) {
    if (printProtocolMessages) print(message);

    const parsedMessage = JSON.parse(message);
    if (parsedMessage.id !== undefined) {
      this.receivedMessages.set(parsedMessage.id, parsedMessage);
    }

    this.dispatchMessage(parsedMessage);
  }

  sendMessage(message) {
    if (printProtocolMessages) print(message);
    send(message);
  }

  sendMessageForMethodChecked(method, params) {
    const {msgid, msg} = this.createMessage(method, params);
    this.sendMessage(msg);
    this.takeReplyChecked(msgid);
  }

  takeReplyChecked(msgid) {
    const reply = this.receivedMessages.get(msgid);
    assertTrue(reply !== undefined);
    this.receivedMessages.delete(msgid);
    return reply;
  }

  setBreakPointAtLocation(scriptid, loc, opt_condition) {
    const params = { location :
                       { scriptId : scriptid.toString(),
                         lineNumber : loc.line,
                         columnNumber : loc.column,
                       },
                     condition : opt_condition,
                   };

    const {msgid, msg} = this.createMessage("Debugger.setBreakpoint", params);
    this.sendMessage(msg);

    const reply = this.takeReplyChecked(msgid);
    assertTrue(reply.result !== undefined);
    const breakid = reply.result.breakpointId;
    assertTrue(breakid !== undefined);

    this.breakpoints.add(breakid);

    return breakid;
  }

  execStatePrepareStep(action) {
    switch(action) {
      case this.StepAction.StepOut: this.stepOut(); break;
      case this.StepAction.StepNext: this.stepOver(); break;
      case this.StepAction.StepIn: this.stepInto(); break;
      default: %AbortJS("Unsupported StepAction"); break;
    }
  }

  execStateScopeType(type) {
    switch (type) {
      case "global": return this.ScopeType.Global;
      case "local": return this.ScopeType.Local;
      case "with": return this.ScopeType.With;
      case "closure": return this.ScopeType.Closure;
      case "catch": return this.ScopeType.Catch;
      case "block": return this.ScopeType.Block;
      case "script": return this.ScopeType.Script;
      default: %AbortJS("Unexpected scope type");
    }
  }

  execStateScopeObjectProperty(serialized_scope, prop) {
    let found = null;
    for (let i = 0; i < serialized_scope.length; i++) {
      const elem = serialized_scope[i];
      if (elem.name == prop) {
        found = elem;
        break;
      }
    }

    if (found == null) return { isUndefined : true }

    const val = { value : () => found.value.value };
    return { value : () => val,
             isUndefined : () => found.value.type == "undefined"
           };
  }

  // Returns an array of property descriptors of the scope object.
  // This is in contrast to the original API, which simply passed object
  // mirrors.
  execStateScopeObject(obj) {
    const serialized_scope = this.getProperties(obj.objectId);
    const scope = this.propertiesToObject(serialized_scope);
    return { value : () => scope,
             property : (prop) =>
                 this.execStateScopeObjectProperty(serialized_scope, prop)
           };
  }

  setVariableValue(frame, scope_index, name, value) {
    const frameid = frame.callFrameId;
    const {msgid, msg} = this.createMessage(
        "Debugger.setVariableValue",
        { callFrameId : frameid,
          scopeNumber : scope_index,
          variableName : name,
          newValue : { value : value }
        });
    this.sendMessage(msg);
    this.takeReplyChecked(msgid);
  }

  execStateScope(frame, scope_index) {
    const scope = frame.scopeChain[scope_index];
    return { scopeType : () => this.execStateScopeType(scope.type),
             scopeObject : () => this.execStateScopeObject(scope.object),
             setVariableValue :
                (name, value) => this.setVariableValue(frame, scope_index,
                                                       name, value)
           };
  }

  // Takes a list of properties as produced by getProperties and turns them
  // into an object.
  propertiesToObject(props) {
    const obj = {}
    props.forEach((elem) => {
      const key = elem.name;

      let value;
      if (elem.value) {
        // Some properties (e.g. with getters/setters) don't have a value.
        switch (elem.value.type) {
          case "undefined": value = undefined; break;
          default: value = elem.value.value; break;
        }
      }

      obj[key] = value;
    })

    return obj;
  }

  getProperties(objectId) {
    const {msgid, msg} = this.createMessage(
        "Runtime.getProperties", { objectId : objectId });
    this.sendMessage(msg);
    const reply = this.takeReplyChecked(msgid);
    return reply.result.result;
  }

  getLocalScopeDetails(frame) {
    const scopes = frame.scopeChain;
    for (let i = 0; i < scopes.length; i++) {
      const scope = scopes[i]
      if (scope.type == "local") {
        return this.getProperties(scope.object.objectId);
      }
    }

    return undefined;
  }

  execStateFrameLocalCount(frame) {
    const scope_details = this.getLocalScopeDetails(frame);
    return scope_details ? scope_details.length : 0;
  }

  execStateFrameLocalName(frame, index) {
    const scope_details = this.getLocalScopeDetails(frame);
    if (index < 0 || index >= scope_details.length) return undefined;
    return scope_details[index].name;
  }

  execStateFrameLocalValue(frame, index) {
    const scope_details = this.getLocalScopeDetails(frame);
    if (index < 0 || index >= scope_details.length) return undefined;

    const local = scope_details[index];

    let localValue;
    switch (local.value.type) {
      case "undefined": localValue = undefined; break;
      default: localValue = local.value.value; break;
    }

    return { value : () => localValue };
  }

  reconstructRemoteObject(obj) {
    let value = obj.value;
    if (obj.type == "object") {
      if (obj.subtype == "error") {
        const desc = obj.description;
        switch (obj.className) {
          case "EvalError": throw new EvalError(desc);
          case "RangeError": throw new RangeError(desc);
          case "ReferenceError": throw new ReferenceError(desc);
          case "SyntaxError": throw new SyntaxError(desc);
          case "TypeError": throw new TypeError(desc);
          case "URIError": throw new URIError(desc);
          default: throw new Error(desc);
        }
      } else if (obj.subtype == "array") {
        const array = [];
        const props = this.propertiesToObject(
            this.getProperties(obj.objectId));
        for (let i = 0; i < props.length; i++) {
          array[i] = props[i];
        }
        value = array;
      }
    }

    return { value : () => value,
             isUndefined : () => obj.type == "undefined"
           };
  }

  evaluateOnCallFrame(frame, expr) {
    const frameid = frame.callFrameId;
    const {msgid, msg} = this.createMessage(
        "Debugger.evaluateOnCallFrame",
        { callFrameId : frameid,
          expression : expr
        });
    this.sendMessage(msg);
    const reply = this.takeReplyChecked(msgid);

    const result = reply.result.result;
    return this.reconstructRemoteObject(result);
  }

  execStateFrameRestart(frame) {
    const frameid = frame.callFrameId;
    const {msgid, msg} = this.createMessage(
        "Debugger.restartFrame", { callFrameId : frameid });
    this.sendMessage(msg);
    this.takeReplyChecked(msgid);
  }

  execStateFrame(frame) {
    const scriptid = parseInt(frame.location.scriptId);
    const line = frame.location.lineNumber;
    const column = frame.location.columnNumber;
    const loc = %ScriptLocationFromLine2(scriptid, line, column, 0);
    const func = { name : () => frame.functionName };

    function allScopes() {
      const scopes = [];
      for (let i = 0; i < frame.scopeChain.length; i++) {
        scopes.push(this.execStateScope(frame, i));
      }
      return scopes;
    };

    return { sourceColumn : () => loc.column,
             sourceLine : () => loc.line + 1,
             sourceLineText : () => loc.sourceText,
             evaluate : (expr) => this.evaluateOnCallFrame(frame, expr),
             functionName : () => frame.functionName,
             func : () => func,
             localCount : () => this.execStateFrameLocalCount(frame),
             localName : (ix) => this.execStateFrameLocalName(frame, ix),
             localValue: (ix) => this.execStateFrameLocalValue(frame, ix),
             restart : () => this.execStateFrameRestart(frame),
             scopeCount : () => frame.scopeChain.length,
             scope : (index) => this.execStateScope(frame, index),
             allScopes : allScopes.bind(this)
           };
  }

  eventDataException(params) {
    switch (params.data.type) {
      case "string": {
        return params.data.value;
      }
      case "object": {
        const props = this.getProperties(params.data.objectId);
        return this.propertiesToObject(props);
      }
      default: {
        return undefined;
      }
    }
  }

  eventDataScriptSource(id) {
    const {msgid, msg} = this.createMessage(
        "Debugger.getScriptSource", { scriptId : id });
    this.sendMessage(msg);
    const reply = this.takeReplyChecked(msgid);
    return reply.result.scriptSource;
  }

  eventDataScriptSetSource(id, src) {
    const {msgid, msg} = this.createMessage(
        "Debugger.setScriptSource", { scriptId : id, scriptSource : src });
    this.sendMessage(msg);
    this.takeReplyChecked(msgid);
  }

  eventDataScript(params) {
    const id = params.scriptId;
    const name = params.url ? params.url : undefined;

    return { id : () => id,
             name : () => name,
             source : () => this.eventDataScriptSource(id),
             setSource : (src) => this.eventDataScriptSetSource(id, src)
           };
  }

  // --- Message handlers. -----------------------------------------------------

  dispatchMessage(message) {
    const method = message.method;
    if (method == "Debugger.paused") {
      this.handleDebuggerPaused(message);
    } else if (method == "Debugger.scriptParsed") {
      this.handleDebuggerScriptParsed(message);
    } else if (method == "Debugger.scriptFailedToParse") {
      this.handleDebuggerScriptFailedToParse(message);
    }
  }

  handleDebuggerPaused(message) {
    const params = message.params;

    var debugEvent;
    switch (params.reason) {
      case "exception":
      case "promiseRejection":
        debugEvent = this.DebugEvent.Exception;
        break;
      default:
        // TODO(jgruber): More granularity.
        debugEvent = this.DebugEvent.Break;
        break;
    }

    // Skip break events in this file.
    if (params.callFrames[0].location.scriptId == this.thisScriptId) return;

    // TODO(jgruber): Arguments as needed.
    let execState = { frames : params.callFrames,
                      prepareStep : this.execStatePrepareStep.bind(this),
                      frame : (index) => this.execStateFrame(
                          index ? params.callFrames[index]
                                : params.callFrames[0]),
                      frameCount : () => params.callFrames.length
                    };

    let eventData = this.execStateFrame(params.callFrames[0]);
    if (debugEvent == this.DebugEvent.Exception) {
      eventData.uncaught = () => params.data.uncaught;
      eventData.exception = () => this.eventDataException(params);
    }

    this.invokeListener(debugEvent, execState, eventData);
  }

  handleDebuggerScriptParsed(message) {
    const params = message.params;
    let eventData = { scriptId : params.scriptId,
                      script : () => this.eventDataScript(params),
                      eventType : this.DebugEvent.AfterCompile
                    }

    // TODO(jgruber): Arguments as needed. Still completely missing exec_state,
    // and eventData used to contain the script mirror instead of its id.
    this.invokeListener(this.DebugEvent.AfterCompile, undefined, eventData,
                        undefined);
  }

  handleDebuggerScriptFailedToParse(message) {
    const params = message.params;
    let eventData = { scriptId : params.scriptId,
                      script : () => this.eventDataScript(params),
                      eventType : this.DebugEvent.CompileError
                    }

    // TODO(jgruber): Arguments as needed. Still completely missing exec_state,
    // and eventData used to contain the script mirror instead of its id.
    this.invokeListener(this.DebugEvent.CompileError, undefined, eventData,
                        undefined);
  }

  invokeListener(event, exec_state, event_data, data) {
    if (this.listener) {
      this.listener(event, exec_state, event_data, data);
    }
  }
}

// Simulate the debug object generated by --expose-debug-as debug.
var debug = { instance : undefined };

Object.defineProperty(debug, 'Debug', { get: function() {
  if (!debug.instance) {
    debug.instance = new DebugWrapper();
    debug.instance.enable();
  }
  return debug.instance;
}});

Object.defineProperty(debug, 'ScopeType', { get: function() {
  const instance = debug.Debug;
  return instance.ScopeType;
}});
