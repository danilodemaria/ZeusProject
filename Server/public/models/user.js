const mongoose = require('mongoose');
const UserSchema = new mongoose.Schema({
    nivel{
        type: String,
        require: true,
    },
    pressao{
        type: String,
        require: true,
    },
    data{
        type: Date,
        default: Date.now,
    },
});

const User = mongoose.model('User', UserSchema);

module.exports = User;