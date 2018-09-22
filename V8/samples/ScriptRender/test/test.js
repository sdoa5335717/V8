

//var three = require('./d3d.js');


//var renderer = new THREE();

//import {GetRenderContext} from './d3d'
//var r = GetRenderContext();
moudle('d3d.js');

var point1 = [-10, -10, 1.0, 0xFFFF0000];
var point2 = [-10, 10 ,1.0, 0xFF00FF00];
var point3 = [10,10,1.0, 0xFF0000FF];
var point4 = [10, -10, 1.0, 0xFFFFFF00];

var index = [0, 1, 2, 0, 2, 3];

var arr = new Array(point1, point2, point3, point4);

// 构建网格
//render.buildMesh();
//var index = new Array(0,1,2);
//var usindex = true;

render.setRenderParam(arr, index);


//var render = THREE.GetRenderContext();



