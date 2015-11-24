import numpy as np
from matplotlib import pyplot as plt
from matplotlib.widgets import Slider, CheckButtons
from matplotlib.colors import LogNorm, SymLogNorm, Normalize
class Visualizer:
    def __init__(self, field, fieldname, halospec=None):
        """Initializes a visualization instance, that is a windows with a field
        field is a 3D numpy array
        fieldname is a string with the name of the field
        halospec is a 2x2 array with the definition of the halo size

        After this call the window is shown
        """
        self.field = field
        self.fieldname = fieldname

        # Register halo information
        if halospec is None:
            halospec = [[3,3],[3,3]]
        self.istart = halospec[0][0]
        self.iend = field.shape[0] - halospec[0][1]
        self.jstart = halospec[1][0]
        self.jend = field.shape[1] - halospec[1][1]
        self.plotHalo = True
        self.plotLogLog = False
    
        self.curklevel = 0

        self.figure = plt.figure()

        # Slider
        slideraxes = plt.axes([0.15, 0.02, 0.5, 0.03], axisbg='lightgoldenrodyellow')
        self.slider = Slider(slideraxes, 'K level', 0, field.shape[2]-1, valinit=0)
        self.slider.valfmt = '%2d'
        self.slider.set_val(0)
        self.slider.on_changed(self.updateSlider)
        
        # CheckButton
        self.cbaxes = plt.axes([0.8, -.04, 0.12, 0.15])
        self.cbaxes.set_axis_off()
        self.cb = CheckButtons(self.cbaxes, ('Halo', 'Logscale'), (self.plotHalo, self.plotLogLog))
        self.cb.on_clicked(self.updateButton)

        # Initial plot
        self.fieldaxes = self.figure.add_axes([0.1, 0.15, 0.9, 0.75])
        self.collection = plt.pcolor(self._getField(), axes=self.fieldaxes)
        self.colorbar = plt.colorbar()
        self.fieldaxes.set_xlim(right=self._getField().shape[1])
        self.fieldaxes.set_ylim(top=self._getField().shape[0])
        plt.xlabel('i')
        plt.ylabel('j')
        self.title = plt.title('%s - Level 0' % (fieldname,))
        
        plt.show(block=False)

    def updateSlider(self, val):
        if val == self.curklevel:
            return
        self.curklevel = round(val)
        self.title.set_text('%s - Level %d' % (self.fieldname, self.curklevel))

        # Draw new field level
        field = self._getField()
        size = field.shape[0] * field.shape[1]
        array = field.reshape(size)
        self.collection.set_array(array)

        self.colorbar.set_clim(vmin=field.min(), vmax=field.max())
        self.collection.set_clim(vmin=field.min(), vmax=field.max())
        self.colorbar.update_normal(self.collection)
        self.figure.canvas.draw_idle()

    def updateButton(self, label):
        if label == 'Halo':
            self.plotHalo = not self.plotHalo
        if label == 'Logscale':
            self.plotLogLog = not self.plotLogLog
        self.updatePlot()


    def updatePlot(self):
        
        # Redraw field
        self.collection.remove()
        field = self._getField()
        if (self.plotLogLog):
            minvalue = field.min()
            norm=SymLogNorm(linthresh=1e-10)
            self.collection = plt.pcolor(field, axes=self.fieldaxes,
                                         norm=norm)
            self.colorbar.set_clim(vmin=minvalue, vmax=field.max())
        else:
            self.collection = plt.pcolor(field, axes=self.fieldaxes)
            self.colorbar.set_clim(vmin=field.min(), vmax=field.max())
            self.colorbar.set_norm(norm=Normalize(vmin=field.min(), vmax=field.max()))
        self.fieldaxes.set_xlim(right=field.shape[1])
        self.fieldaxes.set_ylim(top=field.shape[0])

        self.colorbar.update_normal(self.collection)
        self.figure.canvas.draw_idle()
    
    def _getField(self):
        if self.plotHalo:
            return np.rot90(self.field[:,:,self.curklevel])
        else:
            return np.rot90(self.field[self.istart:self.iend , self.jstart:self.jend , self.curklevel])



