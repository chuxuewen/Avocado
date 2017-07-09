import { Div, Button } from ':gui';
import Mynavpage from 'library.jsx';

export const vx = (
  <Mynavpage title="Zlib" source=($(__filename))>
    <Div width="full">
      <Button class="long_btn">OK</Button>
    </Div>
  </Mynavpage>
)