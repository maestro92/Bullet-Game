#include "text.h"

text::text(float w, float h, vector<unsigned int>& ch)
{
    width = w;
    height = h;
    characters = ch;

}

void text::drawText(coordinate pos, coordinate rot, const char* tex)
{
    // make sure translate and rotate does not mess everything else up
    glPushMatrix();
        glTranslatef(pos.x, pos.y, pos.z);
        glRotatef(rot.x, 1.0, 0,   0);
        glRotatef(rot.y, 0,   1.0, 0);
        glRotatef(rot.z, 0,   0,   1.0);

        int num_char = 0;
        for(int i=0; i<strlen(tex); i++)
        {
            // handling space
            if(tex[i] == ' ')
            {
                num_char++;
                glTranslatef(width, 0, 0);
                continue;
            }

            // handling enter
            if(tex[i] == '\n')
            {
                glTranslatef(-num_char * width, -height, 0);
                num_char = 0;
                continue;
            }
            num_char++;
            // typecast the specific alphabets into ASCII numbers
            glCallList(characters.at((int)tex[i]-65));
            // makespace for the next character
            glTranslatef(width, 0, 0);
        }
    glPopMatrix();

}
