//
// Created by templechen on 2019-09-02.
//

#include "TemplateBaseFilter.h"
#include "../image/ImageCreator.h"
#include "../base/matrix_util.h"
#include "../base/gl_utils.h"

#define GET_STR(x) #x

static const char *VERTEX_SHADER_STR = GET_STR(
    attribute vec4 aPosition;
    uniform mat4 uTextureMatrix;
    attribute vec4 aTextureCoordinate;
    varying vec2 vTextureCoord;
    uniform mat4 uMVPMatrix;
    void main()
    {
        vTextureCoord = (uTextureMatrix * aTextureCoordinate).xy;
        gl_Position = uMVPMatrix * aPosition;
    }
);

static const char *FRAGMENT_SHADER_STR = GET_STR(
    precision highp float;
    varying vec2 vTextureCoord;
    uniform sampler2D uTextureY;
    uniform sampler2D uTextureU;
    uniform sampler2D uTextureV;
    void main()
    {
        float y = texture2D(uTextureY, vTextureCoord).r;
        float u = texture2D(uTextureU, vTextureCoord).r - 0.5;
        float v = texture2D(uTextureV, vTextureCoord).r - 0.5;
        float r = y + 1.402 * v;
        float g = y - 0.344 * u - 0.714 * v;
        float b = y + 1.772 * u;
        gl_FragColor = vec4(r,g,b,1.0);
    }
);

TemplateBaseFilter::TemplateBaseFilter() {
    vertexShader = loadShader(GL_VERTEX_SHADER, VERTEX_SHADER_STR);
    fragmentShader = loadShader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER_STR);
    program = createShaderProgram(vertexShader, fragmentShader);
    yTexture = createTexture(GL_TEXTURE_2D);
    uTexture = createTexture(GL_TEXTURE_2D);
    vTexture = createTexture(GL_TEXTURE_2D);

    imageCreator = new ImageCreator();

    uCoordMatrix = new ESMatrix();
    setIdentityM(uCoordMatrix);
    textureMatrix = new ESMatrix();
    setIdentityM(textureMatrix);
}

TemplateBaseFilter::~TemplateBaseFilter() {
    imageCreator->releaseImage();
    delete imageCreator;

    glDeleteProgram(program);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    auto textures = new GLuint[3];
    textures[0] = yTexture;
    textures[1] = uTexture;
    textures[2] = vTexture;
    glDeleteTextures(1, textures);
}

void TemplateBaseFilter::doFrame() {
    glUseProgram(program);

    GLint vertexCount = sizeof(vertex) / (sizeof(vertex[0]) * 2);
    aPositionLocation = glGetAttribLocation(program, aPosition);
    glEnableVertexAttribArray(aPositionLocation);
    glVertexAttribPointer(aPositionLocation, 2, GL_FLOAT, GL_FALSE, 8, vertex);

    aTextureCoordinateLocation = glGetAttribLocation(program, aTextureCoordinate);
    glEnableVertexAttribArray(aTextureCoordinateLocation);
    glVertexAttribPointer(aTextureCoordinateLocation, 2, GL_FLOAT, GL_FALSE, 8, texture);

    uTextureMatrixLocation = glGetUniformLocation(program, uTextureMatrix);
    glUniformMatrix4fv(uTextureMatrixLocation, 1, GL_FALSE, this->textureMatrix->m);

    uMVPMatrixLocation = glGetUniformLocation(program, uMVPMatrix);
    glUniformMatrix4fv(uMVPMatrixLocation, 1, GL_FALSE, this->uCoordMatrix->m);

    AVFrame *avFrame = imageCreator->readImage("sdcard/test.jpeg");

    uTextureYLocation = glGetUniformLocation(program, uTextureY);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, yTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, avFrame->width, avFrame->height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                 avFrame->data[0]);
    glUniform1i(uTextureYLocation, 0);

    uTextureULocation = glGetUniformLocation(program, uTextureU);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, uTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, avFrame->width / 2, avFrame->height / 2, 0, GL_LUMINANCE,
                 GL_UNSIGNED_BYTE,
                 avFrame->data[1]);
    glUniform1i(uTextureULocation, 1);

    uTextureVLocation = glGetUniformLocation(program, uTextureV);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, vTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, avFrame->width / 2, avFrame->height / 2, 0, GL_LUMINANCE,
                 GL_UNSIGNED_BYTE,
                 avFrame->data[2]);
    glUniform1i(uTextureVLocation, 2);

    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glDisableVertexAttribArray(aPositionLocation);
    glDisableVertexAttribArray(aTextureCoordinateLocation);
    glBindTexture(GL_TEXTURE_2D, 0);
}
