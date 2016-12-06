var Matrix = {
	// Set the rotation components of a 4x4 matrix
    setRotationMatrix: function(angle, x, y, z)
    {
    	var matrix = new Array(16);
        var radians = (angle * Math.PI) / 180.0;
        var c = Math.cos(radians);
        var s = Math.sin(radians);
        var c1 = 1.0 - Math.cos(radians);
        
        var length = Math.sqrt(x * x + y * y + z * z);
        var u = new Array(3);
        u[0] = x / length;
        u[1] = y / length;
        u[2] = z / length;
        
        for (var i = 0; i < 16; i++) {
            matrix[i] = 0.0;
        }
        
        matrix[15] = 1.0;
        
        for (var i = 0; i < 3; i++) {
            matrix[i * 4 + (i + 1) % 3] = u[(i + 2) % 3] * s;
            matrix[i * 4 + (i + 2) % 3] = -u[(i + 1) % 3] * s;
        }
        
        for (var i = 0; i < 3; i++) {
            for (var j = 0; j < 3; j++) {
                matrix[i * 4 + j] += c1 * u[i] * u[j] + (i == j ? c : 0.0);
            }
        }
        return matrix;
    },
    // Set the translation components of a 4x4 matrix
    translatePoseMatrix: function(x, y, z, matrix)
    {
        if (matrix) {
            // matrix * translate_matrix
            matrix[12] += (matrix[0] * x + matrix[4] * y + matrix[8]  * z);
            matrix[13] += (matrix[1] * x + matrix[5] * y + matrix[9]  * z);
            matrix[14] += (matrix[2] * x + matrix[6] * y + matrix[10] * z);
            matrix[15] += (matrix[3] * x + matrix[7] * y + matrix[11] * z);
        }
        return matrix;
    },
    // Apply a rotation
    rotatePoseMatrix: function(angle, x, y, z, matrix)
    {
        if (matrix) {
            var rotate_matrix = Matrix.setRotationMatrix(angle, x, y, z);
            // matrix * scale_matrix
            matrix = Matrix.multiplyMatrix(matrix, rotate_matrix);
        }
        return matrix;
    },
    // Apply a scaling transformation
    scalePoseMatrix: function(x, y, z, matrix)
    {
        if (matrix) {
            // matrix * scale_matrix
            matrix[0]  *= x;
            matrix[1]  *= x;
            matrix[2]  *= x;
            matrix[3]  *= x;
            
            matrix[4]  *= y;
            matrix[5]  *= y;
            matrix[6]  *= y;
            matrix[7]  *= y;
            
            matrix[8]  *= z;
            matrix[9]  *= z;
            matrix[10] *= z;
            matrix[11] *= z;
        }
        return matrix;
    },
    // Multiply the two matrices A and B and write the result to C
    multiplyMatrix: function(matrixA, matrixB)
    {
        var aTmp = new Array(16);
        var matrixC = new Array(16);
        
        for (var i = 0; i < 4; i++) {
            for (var j = 0; j < 4; j++) {
                aTmp[j * 4 + i] = 0.0;
                
                for (var k = 0; k < 4; k++) {
                    aTmp[j * 4 + i] += matrixA[k * 4 + i] * matrixB[j * 4 + k];
                }
            }
        }
        
        for (i = 0; i < 16; i++) {
            matrixC[i] = aTmp[i];
        }
        return matrixC;
    },
    calculateRotationMatrix: function(matrix) {
        var xRotation = Math.atan2(matrix[9], matrix[10])*180.0/Math.PI;
        var yRotation = Math.atan2(-matrix[8], Math.sqrt(matrix[9]*matrix[9] + matrix[10]*matrix[10]))*180.0/Math.PI;
        var zRotation = Math.atan2(matrix[5], matrix[0])*180.0/Math.PI;
        return cc.math.vec3(xRotation, yRotation, zRotation);
    },
};